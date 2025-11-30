// Fill out your copyright notice in the Description page of Project Settings.

#include "SignalRoomSet.h"

const FSignalRoomArchetype* USignalRoomSet::FindArchetype(ESignalRoomType RoomType) const
{
    for (const FSignalRoomArchetype& Archetype : RoomTypes)
    {
        if (Archetype.RoomType == RoomType)
        {
            return &Archetype;
        }
    }
    return nullptr;
}
