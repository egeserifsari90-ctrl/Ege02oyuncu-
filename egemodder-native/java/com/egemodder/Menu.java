package com.egemodder;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.os.Handler;
import android.os.Looper;
import android.text.InputType;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import java.lang.reflect.Field;
import java.lang.ref.WeakReference;
import java.util.Locale;

/** Experimental Egemodder menu attached to the host Unity Activity. */
public final class Menu {
  private static final Handler MAIN=new Handler(Looper.getMainLooper());
  private static WeakReference<Activity> attached=new WeakReference<Activity>(null);
  private static boolean nativeReady;
  private static int attempts;
  static {
    try {System.loadLibrary("egemodder");nativeReady=true;}
    catch(Throwable ignored){nativeReady=false;}
  }
  private Menu(){}
  public static void start(){
    MAIN.post(new Runnable(){
      @Override public void run(){
        Activity a=getActivity();
        if(a==null||a.isFinishing()){
          if(++attempts<120)MAIN.postDelayed(this,500);
          return;
        }
        if(attached.get()!=a){
          attached=new WeakReference<Activity>(a);
          addBubble(a);
        }
      }
    });
  }
  private static Activity getActivity(){
    try{
      Class<?> k=Class.forName("com.unity3d.player.UnityPlayer");
      Field field=k.getDeclaredField("currentActivity");
      field.setAccessible(true);
      Object a=field.get(null);
      if(a instanceof Activity)return (Activity)a;
    }catch(Throwable ignored){}
    return null;
  }
  private static int dp(Activity a,int n){
    return Math.round(n*a.getResources().getDisplayMetrics().density);
  }
  private static void addBubble(final Activity a){
    View root=a.findViewById(android.R.id.content);
    if(!(root instanceof FrameLayout))return;
    FrameLayout parent=(FrameLayout)root;
    Button btn=new Button(a);
    btn.setAllCaps(false);
    btn.setText("🐻 Egemodder");
    btn.setTextColor(Color.WHITE);
    GradientDrawable shape=new GradientDrawable();
    shape.setColor(Color.rgb(31,45,72));
    shape.setCornerRadius(dp(a,16));
    btn.setBackground(shape);
    FrameLayout.LayoutParams p=new FrameLayout.LayoutParams(dp(a,145),dp(a,48),Gravity.TOP|Gravity.LEFT);
    p.topMargin=dp(a,35);p.leftMargin=dp(a,12);
    parent.addView(btn,p);
    btn.setOnClickListener(new View.OnClickListener(){
      @Override public void onClick(View v){showPin(a);}
    });
  }
  private static void showPin(final Activity a){
    final EditText pin=new EditText(a);
    pin.setHint("Şifre");
    pin.setInputType(InputType.TYPE_CLASS_NUMBER|InputType.TYPE_NUMBER_VARIATION_PASSWORD);
    new AlertDialog.Builder(a).setTitle("🐻 Egemodder 1234")
      .setMessage("Super Bear Adventure 13.0.8 deneysel mod menüsü")
      .setView(pin).setNegativeButton("Vazgeç",null)
      .setPositiveButton("Aç",new DialogInterface.OnClickListener(){
        @Override public void onClick(DialogInterface d,int which){
          if("1234".equals(pin.getText().toString()))showMenu(a);
          else Toast.makeText(a,"Yanlış şifre",Toast.LENGTH_SHORT).show();
        }
      }).show();
  }
  private static CheckBox check(Activity a,String title,boolean enabled){
    CheckBox b=new CheckBox(a);
    b.setText(title);b.setChecked(enabled);
    b.setPadding(dp(a,8),dp(a,8),dp(a,8),dp(a,8));
    return b;
  }
  private static void feature(Activity a,int id,float value){
    if(!nativeReady){Toast.makeText(a,"Yerel mod kütüphanesi yüklenemedi",Toast.LENGTH_LONG).show();return;}
    try{
      if(nativeSetFeature(id,value)!=0)
        Toast.makeText(a,"Bu seçenek kabul edilmedi",Toast.LENGTH_SHORT).show();
    }catch(Throwable ignored){
      Toast.makeText(a,"Mod API bağlantısı hatalı",Toast.LENGTH_SHORT).show();
    }
  }
  private static void showMenu(final Activity a){
    ScrollView scroll=new ScrollView(a);
    LinearLayout list=new LinearLayout(a);
    list.setOrientation(LinearLayout.VERTICAL);
    list.setPadding(dp(a,12),dp(a,8),dp(a,12),dp(a,12));
    scroll.addView(list);
    final CheckBox jump=check(a,"Sınırsız çift zıplama (deneysel)",true);
    jump.setOnClickListener(new View.OnClickListener(){
      @Override public void onClick(View v){feature(a,2,jump.isChecked()?1f:0f);}
    });
    list.addView(jump);
    final CheckBox hover=check(a,"Havada kal (yerçekimi kapalı)",false);
    hover.setOnClickListener(new View.OnClickListener(){
      @Override public void onClick(View v){feature(a,1,hover.isChecked()?1f:0f);}
    });
    list.addView(hover);
    final CheckBox noclip=check(a,"No Clip (zeminden de geçebilir)",false);
    noclip.setOnClickListener(new View.OnClickListener(){
      @Override public void onClick(View v){feature(a,3,noclip.isChecked()?1f:0f);}
    });
    list.addView(noclip);
    final TextView label=new TextView(a);
    label.setText("Oyun hızı: 1.00x");
    list.addView(label);
    SeekBar speed=new SeekBar(a);
    speed.setMax(275);speed.setProgress(75);
    speed.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
      @Override public void onProgressChanged(SeekBar bar,int n,boolean user){
        float factor=(n+25)/100.f;
        label.setText(String.format(Locale.US,"Oyun hızı: %.2fx",factor));
        if(user)feature(a,4,factor);
      }
      @Override public void onStartTrackingTouch(SeekBar b){}
      @Override public void onStopTrackingTouch(SeekBar b){}
    });
    list.addView(speed);
    TextView warn=new TextView(a);
    warn.setText("Deneysel: henüz oyun içinde doğrulanmadı. No Clip zeminden düşürebilir.");
    warn.setPadding(0,dp(a,12),0,0);
    list.addView(warn);
    new AlertDialog.Builder(a).setTitle("Egemodder – 13.0.8")
      .setView(scroll).setPositiveButton("Kapat",null).show();
  }
  private static native int nativeSetFeature(int id,float value);
}
