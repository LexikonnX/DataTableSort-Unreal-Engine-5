#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTableSortBlueprintLibrary.generated.h"

class UDataTable;

UCLASS()
class DATATABLESORT_API UDataTableSortBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Data Table|Sorting", meta = (DisplayName = "Get Sorted Data Table Row Names", Keywords = "sort data table rows column order", AdvancedDisplay = "bDescending"))
    static TArray<FName> GetSortedDataTableRowNames(UDataTable* DataTable, FName ColumnName, bool bDescending = false);
};
