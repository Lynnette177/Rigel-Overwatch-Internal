#define WIN32_LEAN_AND_MEAN
#pragma once

// needed for utf-8 url encoding -  see comments at https://alfps.wordpress.com/2011/12/08/unicode-part-2-utf-8-stream-mode/
// win10 build 17134 (April 2018 Update) and later users can use setlocale(LC_ALL, ".UTF8") see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale?view=msvc-160
#pragma execution_character_set( "utf-8" ) 

#include <string>
#include <windows.h>
#include <WinInet.h>
#include <Winineti.h>
#include <stdio.h>
#include <cstdint>
#include <map>
#include <regex>
#include <initializer_list>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <codecvt>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <urlmon.h>




namespace wrap {



	struct Comparator {
		bool operator() (const std::string& s1, const std::string& s2) const {
			std::string str1;
			std::string str2;
			//std::transform(s1.begin(), s1.end(), str1.begin(), tolower);
			for (auto& c : s1)
			{
				str1 += std::tolower(c);
			}
			for (auto& c : s2)
			{
				str2 += std::tolower(c);
			}

			//std::transform(s2.begin(), s2.end(), str2.begin(), tolower);
			return  str1 < str2;
		}
	};

	struct Body { //used for postdata
		std::string body;
	};

	struct File { //used for uploading file multipart post
		std::string file;
	};

	struct Download {
		std::string dl = "dl"; //default is to download file keeping original filename
	};

	struct Method {
		std::string method;
	};

	struct Url {
		std::string adr;
	};

	struct UserAgent {
		std::string usr;
	};

	struct Header {
		std::map<std::string, std::string> hdr;

		Header(std::initializer_list<std::pair<const std::string, std::string> > hdr) :
			hdr(hdr) {
		}
	};

	struct Parameters {
		std::map<std::string, std::string> p;

		Parameters(std::initializer_list<std::pair<const std::string, std::string> > p) :
			p(p) {
		}
	};

	struct Cookies {
		std::map<std::string, std::string> c;
		Cookies(std::initializer_list<std::pair<const std::string, std::string> > c) :
			c(c) {
		}
	};

	struct Payload {
		std::map<std::string, std::string> pd;

		Payload(std::initializer_list<std::pair<const std::string, std::string> > pd) :
			pd(pd) {
		}
	};

	struct Multipart {
		std::map<std::string, std::string> mp;

		Multipart(std::initializer_list<std::pair<const std::string, std::string> > mp) :
			mp(mp) {
		}
	};

	struct Authentication {
		std::string usr;
		std::string pwd;
	};

	struct Bearer {
		std::string token;
	};

	struct Timeout {
		DWORD timeout;
		std::string type = "request"; // connection or request
	};

	struct Options {
		std::map<std::string, std::string> options;
		Options(std::initializer_list<std::pair<const std::string, std::string> > options) :
			options(options) {
		}
	};

	struct Proxy {
		std::string url;
		std::string usr = "";
		std::string pwd = "";
	};

	struct req {

		std::string ua = "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
		std::string Params;
		std::string PostData; // key is type of data (body,payload or file) and value is the data
		std::string Dl;
		std::string Method = "GET";
		std::string Auth;
		std::string Url = "www.example.com";
		wrap::Proxy Proxy;
		std::map <std::string, std::string> cookies_tosend;
		bool cookie_encode = true;
		bool redirect = true;
		bool newsession = true; //if current session should be replaced with a new one via InternetOpen - triggered by proxy or user agent changes
		HINTERNET session_handle = NULL;
		Header Header = { {"",""} };
		DWORD TimeoutConnect = 0;
		DWORD TimeoutRequest = 0;
		DWORD AccessType = INTERNET_OPEN_TYPE_DIRECT;
		void reset() {
			ua = "Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
			Params = "";
			PostData = "";
			Dl = "";
			Method = "GET";
			Auth = "";
			Url = "www.example.com";
			Header = { {"",""} };
			//cookies_tosend.clear(); - currently done at end of prepare_cookies()
			TimeoutConnect = 0;
			TimeoutRequest = 0;
			newsession = false;
			cookie_encode = true;
			redirect = true;

		};
	};



	struct Response {
		std::map <std::string, std::string, Comparator> header;
		std::map <std::string, std::string> sent_headers;
		std::map <std::string, std::string> secinfo;
		std::map <std::string, std::string> cookies;
		std::string url;
		std::string raw;
		std::string text;
		std::string status_code;
		std::string err;
		unsigned long uploaded_bytes = 0;
		unsigned long downloaded_bytes = 0;
		unsigned long redirect_count = 0;
	};



	std::string text_from_html(std::string html);

	//initial req object that will be used to pass params to cpp source function
	extern req toSource;

	void Params(wrap::Parameters s);

	void Params(wrap::Url u);

	void Params(wrap::UserAgent usr);

	void Params(wrap::Header h);

	void Params(wrap::Payload pd);

	void Params(wrap::Multipart mp);

	void Params(wrap::Authentication auth);

	void Params(wrap::Bearer token);

	void Params(wrap::Method m);

	void Params(wrap::Download dl);

	void Params(wrap::Body body);

	void Params(wrap::Cookies c);

	void Params(wrap::File file);

	void Params(wrap::Timeout timeout);

	void Params(wrap::Options options);

	void Params(wrap::Proxy proxy);

	// url, headers, params, postdata, cookies, dl flag, proxies, timeout, method, auth 
	Response httpsreq(req Request);

	//this is to achieve the effect of allowing random params of different types
	//https://stackoverflow.com/questions/67089840
	//not even URL is needed - it's set to example.com by default
	template <typename ...Ts>
	Response HttpsRequest(Ts&& ...args)
	{

		std::initializer_list<int> ignore = { (Params(args), 0)... };
		(void)ignore; // Hack that prevents the initializer_list from being optimized and the "ignore variable is unused" warning
		return httpsreq(toSource);
	};


}