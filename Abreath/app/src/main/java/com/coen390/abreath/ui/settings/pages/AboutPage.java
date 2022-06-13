package com.coen390.abreath.ui.settings.pages;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatDelegate;

import android.content.SharedPreferences;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;

import com.coen390.abreath.R;

import java.util.Objects;

public class AboutPage extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about_page);

        SharedPreferences night = getSharedPreferences("night",0);
        boolean booleanValue = night.getBoolean("night_mode",true);
        if (booleanValue){
            getSupportActionBar().setBackgroundDrawable(new ColorDrawable(getResources()
                    .getColor(R.color.primaryColor)));
        }

        Objects.requireNonNull(getSupportActionBar()).setElevation(0f);
    }
}