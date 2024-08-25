#include "../libs/webview/webview.h"
#include <stdio.h>
#include <stddef.h>

#include "../build/html.c"

#ifdef _WIN32
#include <windows.h>
#endif

void execute(const char *id, const char *req, void *arg);

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
    webview_set_size(w, 750, 475, WEBVIEW_HINT_FIXED);
    webview_set_title(w, "p-please senpai - phoenixthrush");
    webview_bind(w, "execute", execute, NULL);
    webview_set_html(w, (const char *)site_index_html);
    webview_run(w);
    webview_destroy(w);
    return 0;
}