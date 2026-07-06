
/*ARosterInfo - This class is meant to be a way to store CharacterInfos in a way that would be useful for "Squads" "Teams" or other types of rosters
* primarily used in the MissionSubsystem to store a way for assigning players to a particular squad and spawning them together
* Alternatively - it can be used to assign "Players" to a team even without a character info though
*
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "RosterInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemberListChanged, class ACharacterInfo*, TeamMember);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerListChanged, class APlayerState*, Player);

/**
 * 
 */
UCLASS(Blueprintable, abstract)
class FIRSTPERSONMODULE_API ARosterInfo : public AInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roster")
		FString TeamName;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roster")
		TSubclassOf<class AFirstPersonCharacter> TeamPawnClass;
	/*used to help find specific spawn points based on their tag using this name*/
	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "Roster")
		FName SpawnTag;


private:
	UPROPERTY()
		TArray<class ACharacterInfo*> TeamMembers;
	UPROPERTY()
		TArray<class AFirstPersonPlayerState*> Players;

	UPROPERTY()
		class UMissionSubsystem* MissionSubsystem;



public:
	UPROPERTY(BlueprintAssignable)
		FOnMemberListChanged OnTeamMemberAdded;
	UPROPERTY(BlueprintAssignable)
		FOnMemberListChanged OnTeamMemberRemoved;
	UPROPERTY(BlueprintAssignable)
		FOnPlayerListChanged OnPlayerAdded;
	UPROPERTY(BlueprintAssignable)
		FOnPlayerListChanged OnPlayerRemoved;

	//=================================================================================================================================
	//============================================================FUNCTIONS============================================================
	//=================================================================================================================================

public:
	ARosterInfo();


public:
	/*adds a new team member to the roster*/
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void AddTeamMember(class ACharacterInfo* Member);
	/*removes a new team member from the roster*/
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void RemoveTeamMember(class ACharacterInfo* Member);
protected:
	UFUNCTION()
		virtual void ClearOldRoster(class ACharacterInfo* TeamMember);

public:
		/*returns the Team Members of this group*/
	UFUNCTION(BlueprintPure,BlueprintCallable, Category = "Roster")
		TArray<class ACharacterInfo*> GetTeamMembers();


public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Roster")
		TArray<class AFirstPersonPlayerState*> GetPlayers();
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void AddPlayer(class AFirstPersonPlayerState* Player);
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual void RemovePlayer(class AFirstPersonPlayerState* Player);
	UFUNCTION(BlueprintCallable, Category = "Roster")
		virtual bool HasPlayer(class AFirstPersonPlayerState* Player);
	

	
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utilities")
		class UMissionSubsystem* GetMissionSubsystem();

	UFUNCTION()
		virtual void NotifyTeamMemberAdded(class ACharacterInfo* NewTeamMember);
	UFUNCTION()
		virtual void NotifyTeamMemberRemoved(class ACharacterInfo* RemovedTeamMember);
	UFUNCTION()
		virtual void NotifyPlayerAdded(class AFirstPersonPlayerState* Player);
	UFUNCTION()
		virtual void NotifyPlayerRemoved(class AFirstPersonPlayerState* Player);


	
};
