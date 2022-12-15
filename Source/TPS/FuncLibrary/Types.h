
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types.generated.h"


UENUM(BlueprintType)
enum class EMovementState:uint8
{
	AimState UMETA(DisplayName = "Aim State"),
	AimWalkState UMETA(DisplayName = "AimWalk State"),
	SprintRunState UMETA(DisplayName = "SrintRun State"),
	WalkSate UMETA(DisplayName = "Walk State"),
	RunState UMETA(DisplayName = "Run State"),
};

USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")

	float AimSpeedNormal = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeedNormal = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mcvement")
	float RunSpeedNormal = 600.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Movement")
	float AimWalkSpeed = 100.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = " Movement")
	float SprintRunSpeed = 800.0f;
};

UCLASS()
class TPS_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
};
