#include "TPS/Weapon/WeaponDefault.h"

AWeaponDefault::AWeaponDefault()
{
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("No Collision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("No Collision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Shoot Location"));
	ShootLocation->SetupAttachment(RootComponent);
}
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();

	WeaponInit();
}

void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FireTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if(WeaponFiring)
		if (FireTime < 0.0f)
			Fire();
	else
		FireTime -= DeltaTime;
		
}

void AWeaponDefault::WeaponInit()
{
	if(SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
		SkeletalMeshWeapon->DestroyComponent(true);
	
	if(StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
		StaticMeshWeapon->DestroyComponent();
}


void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if(CheckWeaponCanFire())
		WeaponFiring = bIsFire;
	else
		WeaponFiring = false;
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return true;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	FireTime = WeaponSetting.RateOfFire;

	if(ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SPawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		
		if(ProjectileInfo.Progectile)
		{
			//Projectile Init ballistic fire
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();
			
			AActor* SpawnProjectile = GetWorld()->SpawnActor(ProjectileInfo.Progectile,&SpawnLocation,&SPawnRotation);
			AProjectileDefault* Projectile = Cast<AProjectileDefault>(SpawnProjectile);

			if(Projectile)
				Projectile->InitialLifeSpan = 20.0f;
				
		}
	}
	
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	ChangeDispersion();
}

void AWeaponDefault::ChangeDispersion()
{
	
}

