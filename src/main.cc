#include "webview/webview.h"
#include "html.c"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

/* select_folder */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#else
#include <unistd.h>
#endif

std::string select_folder()
{
    std::string folderPath;

#ifdef _WIN32
    BROWSEINFO bi = {0};
    bi.ulFlags = BIF_USENEWUI | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl)
    {
        char path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path))
        {
            folderPath = path;
        }
        CoTaskMemFree(pidl);
    }
#elif __APPLE__
    FILE *fp = popen("osascript -e 'set folderPath to POSIX path of (choose folder)' -e 'return folderPath'", "r");
    if (fp == NULL)
    {
        perror("Error executing AppleScript");
        return "";
    }

    char *line = nullptr;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, fp)) != -1)
    {
        if (read > 0 && line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }
        folderPath = line;
        free(line);
    }

    pclose(fp);
#elif __linux__
    FILE *fp = popen("zenity --file-selection --directory", "r");
    if (fp == NULL)
    {
        perror("Error executing Zenity");
        return "";
    }

    char *line = nullptr;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, fp)) != -1)
    {
        if (read > 0 && line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }
        folderPath = line;
        free(line);
    }

    pclose(fp);
#else
    fprintf(stderr, "Unsupported OS\n");
    return "";
#endif

    return folderPath;
}
/* end */

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#else
int main()
{
#endif
    try
    {
        webview::webview w(true, nullptr);
        w.set_title("Select Folder");
        w.set_size(480, 320, WEBVIEW_HINT_FIXED);

        w.bind(
            "select_folder",
            [&](const std::string &id, const std::string &req, void * /*arg*/)
            {
                std::thread([&, id, req]
                            {
                                std::string folder_path = select_folder();
                                std::string json_result = "\"" + folder_path + "\"";

                                w.resolve(id, 0, json_result.c_str()); })
                    .detach();
            },
            nullptr);

        w.set_html((const char *)site_index_html);
        w.run();
    }
    catch (const webview::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}