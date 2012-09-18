author:dk

libcurl.lib and libcurl.dll were created in visual studio and then linked in to the tools

start with instructions from
http://blog.expandrive.com/2007/07/27/libcurl-on-windows-with-visual-studio-2005/

but skipped steps 3,4 (and 5)

1. Download the source[from curl.haxx.se] and load lib\curllib.vcproj in Visual Studio
2. In curllib project properties->Configuration Properties->C/C++->Code Generation->Runtime Library change the value to Multi-Threaded /MT [assuming a release build] so it correctly builds as a static library


After that 
3. built the project in release mode
4. copied libcurl.lib and libcurl.dll into tools/contrib/lib
5. copied libcurl.dll into dist/bin

included <curl/curl.h> in the source code and used following test code:
  CURL *curl;
  CURLcode res;
  
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
    res = curl_easy_perform(curl);
	if (res == 0) {
		AfxMessageBox("resolved url");
	} else {
		AfxMessageBox("couldn't resolved url");
	}
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }