#include "ProductionBoostPerRecipe.h"

#include "FGBuildableFactory.h"
#include "FGBuildableManufacturer.h"
#include "SML/Public/Patching/NativeHookManager.h"
#include "Registry/ContentTagRegistry.h"

#define LOCTEXT_NAMESPACE "FProductionBoostPerRecipeModule"

DEFINE_LOG_CATEGORY(LogProductionBoostPerRecipe);

thread_local bool bInHook = false;

static const bool DoesRecipePreventBoost(const UObject* worldContext, const TSubclassOf< class UFGRecipe > recipe) {
	const auto registry = UContentTagRegistry::Get(worldContext);
	const auto tags = registry->GetGameplayTagContainerFor(recipe);
	const auto disableBoostTag = FGameplayTag::RequestGameplayTag("ProductionBoostPerRecipe.DisableBoost", true);
	return tags.HasTag(disableBoostTag);
}

// TODO how to declare scope var?
//void Hook_AFGBuildableManufacturer_SetRecipe(auto& scope, AFGBuildableManufacturer* self, TSubclassOf< class UFGRecipe > recipe) {

//}

void FProductionBoostPerRecipeModule::StartupModule()
{
	// TODO SUBSCRIBE_UOBJECT_METHOD ?
	if (!WITH_EDITOR) {
		UE_LOG(LogProductionBoostPerRecipe, Display, TEXT("Applying hooks"));
		// Virtual hooks only work if the child doesn't override it

		AFGBuildableFactory* factory = GetMutableDefault<AFGBuildableFactory>();
		SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableFactory::SetCurrentProductionBoost, factory, [](auto& scope, AFGBuildableFactory* self, float newProductionBoost) {
			UE_LOG(LogProductionBoostPerRecipe, VeryVerbose, TEXT("AFGBuildableFactory::SetCurrentProductionBoost %f"), newProductionBoost);
		});

		// On user add/remove somersloops, also called on recipe change
		// TODO move this inline to its own function for better error reporting - but how to declare scope var?
		SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableFactory::SetPendingProductionBoost, factory, [](auto& scope, AFGBuildableFactory* self, float newPendingProductionBoost) {
			UE_LOG(LogProductionBoostPerRecipe, Display, TEXT("AFGBuildableFactory::SetPendingProductionBoost %f"), newPendingProductionBoost);
			if (bInHook) {
				UE_LOG(LogProductionBoostPerRecipe, Verbose, TEXT(" - Re-entrant call detected, taking no action"));
				return;
			}
			const auto asManufacturer = Cast<AFGBuildableManufacturer>(self);
			if (asManufacturer) {
				const auto recipe = asManufacturer->GetCurrentRecipe();
				if (recipe && DoesRecipePreventBoost(self, recipe)) {
					UE_LOG(LogProductionBoostPerRecipe, Display, TEXT(" - DisableBoost tag found on %s, instead using pending boost of 1.0"), *recipe->GetPathName());
					return scope(self, 1.0f);
				} else {
					UE_LOG(LogProductionBoostPerRecipe, Verbose, TEXT(" - No DisableBoost tag found (or no recipe), allowing boost"));
					scope(self, newPendingProductionBoost); // TODO is this needed? happens anyway right?
				}
			} else {
				UE_LOG(LogProductionBoostPerRecipe, VeryVerbose, TEXT(" - Caller not an AFGBuildableManufacturer, can't check recipe, taking no action"));
			}
			
		});

		AFGBuildableManufacturer* manufacturer = GetMutableDefault<AFGBuildableManufacturer>();
		SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableManufacturer::SetCurrentProductionBoost, manufacturer, [](auto& scope, AFGBuildableManufacturer* self, float newProductionBoost) {
			UE_LOG(LogProductionBoostPerRecipe, VeryVerbose, TEXT("AFGBuildableManufacturer::SetCurrentProductionBoost %f"), newProductionBoost);
		});

		// On Recipe Change in existing machine
		// TODO move this inline to its own function for better error reporting - but how to declare scope var?
		SUBSCRIBE_METHOD_VIRTUAL(AFGBuildableManufacturer::SetRecipe, manufacturer, [](auto& scope, AFGBuildableManufacturer* self, TSubclassOf< class UFGRecipe > recipe) {
			UE_LOG(LogProductionBoostPerRecipe, Display, TEXT("AFGBuildableManufacturer::SetRecipe %s"), *recipe->GetPathName());
			
			bInHook = true;
			if (DoesRecipePreventBoost(self, recipe)) {
				UE_LOG(LogProductionBoostPerRecipe, Display, TEXT(" - DisableBoost tag found, setting boost to 1.0"));
				self->SetPendingProductionBoost(1.0f);
				self->SetCurrentProductionBoost(1.0f); // Required to prevent having a machine with a boost already set, changing recipe, and getting 1 round of boost before pending applied
			} else {
				UE_LOG(LogProductionBoostPerRecipe, Verbose, TEXT(" - No DisableBoost tag found, setting pending boost to max possible (vanilla behavior)"));
				auto maxPossible = self->GetCurrentMaxProductionBoost();
				self->SetPendingProductionBoost(maxPossible);
			}
			bInHook = false;
		});
	}
}

void FProductionBoostPerRecipeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProductionBoostPerRecipeModule, ProductionBoostPerRecipe)