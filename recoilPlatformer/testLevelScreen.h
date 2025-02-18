#pragma once
#ifndef TEST_H
#define TEST_H

int ROWS;
int COLS;

int testLevel[70][70];


void testGameplayScreenInit();

void testGameplayScreenUpdate();

void testGameplayScreenDraw();

void changeScreen(const char* level);
#endif