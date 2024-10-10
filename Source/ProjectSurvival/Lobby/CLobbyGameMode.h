#pragma once

#include "CoreMinimal.h"
#include "CMainGameMode.h"
#include "CLobbyGameMode.generated.h"

/**
* StartGame() - 접속인원 = 커스터마이징 인원 상태에서 클릭 시 Main 맵으로 이동
*/

UCLASS()
class PROJECTSURVIVAL_API ACLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACLobbyGameMode();

protected:
	void PostLogin(APlayerController* NewPlayer) override;
	void Tick(float DeltaSeconds) override;
	
private:
	UFUNCTION()
		void TriggerSeamlessTravel();

public:
	void ReadyPlayer();
	bool CheckPlayer();
	void StartGame();
	

private:
	int32 NumberOfSurvivors = 0;
	int32 NumberOfReadySurvivors = 0;
	
};