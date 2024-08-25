#include "webview.h"
#include <stdio.h>
#include <stddef.h>

#include "build/html.c"

#ifdef _WIN32
#include <windows.h>
#endif

void print_hi(const char *id, const char *req, void *arg)
{
  (void)id;
  (void)req;
  (void)arg;
  printf("hi\n");
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main() {
#endif
  webview_t w = webview_create(0, NULL);
  webview_set_size(w, 350, 500, WEBVIEW_HINT_FIXED);
  webview_bind(w, "printHi", print_hi, NULL);
  webview_set_html(w, (const char *)site_index_html);
  webview_run(w);
  webview_destroy(w);
  return 0;
}