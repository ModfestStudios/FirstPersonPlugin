
/*ARosterInfo - This class is meant to be a way to store CharacterInfos in a way that would be useful for "Squads" "Teams" or other types of rosters
* primarily used in the MissionSubsystem to store a way for assigning players to a particular squad and spawning them together
*
*
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "RosterInfo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemberListChanged, class ACharacterInfo*, TeamMember);

/**
 * 
 */
UCLASS()
class FIRSTPERSONMODULE_API ARosterInfo : public AInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Roster")
		FString TeamName;

private:
	UPROPERTY()
		TArray<class ACharacterInfo*> TeamMembers;

	UPROPERTY()
		class UMissionSubsystem* MissionSubsystem;



public:
	UPROPERTY(BlueprintAssignable)
		FOnMemberListChanged OnTeamMemberAdded;
	UPROPERTY(BlueprintAssignable)
		FOnMemberListChanged OnTeamMemberRemoved;

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



	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Utilities")
		class UMissionSubsystem* GetMissionSubsystem();

	UFUNCTION()
		virtual void NotifyTeamMemberAdded(class ACharacterInfo* NewTeamMember);
	UFUNCTION()
		virtual void NotifyTeamMemberRemoved(class ACharacterInfo* RemovedTeamMember);

	
};
