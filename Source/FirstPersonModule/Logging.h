// Copyright (c) 2022 Pocket Sized Animations

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFirstPersonModule, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirstPersonGameMode, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirstPersonCharacter, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFirstPersonGameMaster, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInventoryModule, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMapSubsystem, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogServerSubsystem, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMission, Log, All);


#define NETMODE_WORLD (((GEngine == nullptr) || (GetWorld() == nullptr)) ? TEXT("") \
        : (GEngine->GetNetMode(GetWorld()) == NM_Client) ? TEXT("[Client] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_ListenServer) ? TEXT("[ListenServer] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer) ? TEXT("[DedicatedServer] ") \
        : TEXT("[Standalone] "))
