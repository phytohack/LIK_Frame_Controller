#pragma once
#include <Arduino.h>

class BasePage {
public:
    static String getHeader(const String &title) {
        String header = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>[Frame] " + title + "</title>";
        header += "<style>";
        header += "body { font-family: Arial, sans-serif; background-color: #f2f2f2; padding: 20px; }";
        header += "h1 { color: #333; }";
        header += "h3 { color: #333; }";
        header += "ul { list-style: none; padding: 0; }";
        header += "li { background: #fff; margin: 5px 0; padding: 10px; border-radius: 4px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
        header += "a { color: #337ab7; text-decoration: none; }";
        header += "a:hover { text-decoration: underline; }";
        header += "button { background: #337ab7; color: #fff; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer; margin: 5px 0; }";
        header += "button:hover { background: #286090; }";
        header += "button:disabled { background: #ccc; color: #666; cursor: not-allowed; }";
        header += "form { margin-bottom: 20px; }";
        header += "</style>";
        header += "</head><body>";
        return header;
    }

    static String getFooter() {
        return "</body></html>";
    }
};
