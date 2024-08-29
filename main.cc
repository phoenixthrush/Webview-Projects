#include "webview/webview.h"

#include <iostream>
#include "html.c"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/,
                   LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int main() {
#endif
  try {
    webview::webview w(false, nullptr);
    w.set_title("Keygen Template");
    w.set_size(350, 500, WEBVIEW_HINT_NONE);
    w.set_html((const char *)site_index_html);
    w.run();
  } catch (const webview::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
