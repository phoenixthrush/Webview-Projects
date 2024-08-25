#include "../libs/webview/webview.h"
#include <stdio.h>
#include <stddef.h>

#include "../build/html.c"

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
    webview_t w = webview_create(0, NULL);
    webview_set_size(w, 950, 650, WEBVIEW_HINT_FIXED); // 750, 475
    webview_set_title(w, "p-please senpai - phoenixthrush");
    webview_bind(w, "call_encrypt", call_encrypt, NULL);
    webview_bind(w, "call_decrypt", call_decrypt, NULL);
    webview_set_html(w, (const char *)site_index_html);
    webview_run(w);
    webview_destroy(w);
    return 0;
}