package com.egemodder.demo;
import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebSettings;
public class MainActivity extends Activity {
  private WebView web;
  @Override public void onCreate(Bundle state) {
    super.onCreate(state);
    web = new WebView(this);
    web.setWebViewClient(new WebViewClient());
    WebSettings settings = web.getSettings();
    settings.setJavaScriptEnabled(true);
    settings.setAllowFileAccess(true);
    setContentView(web);
    web.loadUrl("file:///android_asset/index.html");
  }
  @Override protected void onDestroy(){ if(web!=null) web.destroy(); super.onDestroy(); }
}
