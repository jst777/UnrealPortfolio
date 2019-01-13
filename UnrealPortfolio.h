// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(UnrealPortfolio, Log, All);

#define UPLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define UPLOG_S(Verbosity) UE_LOG(UnrealPortfolio, Verbosity, TEXT("%s"), *UPLOG_CALLINFO)
#define UPLOG(Verbosity, Format, ...) UE_LOG(UnrealPortfolio, Verbosity, TEXT("%s%s"), *UPLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define UPCHECK(Expr, ...) { if(!(Expr)) { UPLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }

