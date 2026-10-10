#include "DataTableSortBlueprintLibrary.h"

#include "Engine/DataTable.h"
#include "UObject/EnumProperty.h"
#include "UObject/UnrealType.h"

DEFINE_LOG_CATEGORY_STATIC(LogDataTableSort, Log, All);

TArray<FName> UDataTableSortBlueprintLibrary::GetSortedDataTableRowNames(UDataTable* DataTable, FName ColumnName, bool bDescending)
{
    if (!IsValid(DataTable) || !DataTable->GetRowStruct() || ColumnName.IsNone())
    {
        return {};
    }

    const FString RequestedName = ColumnName.ToString();
    FProperty* ColumnProperty = DataTable->FindTableProperty(ColumnName);

    if (!ColumnProperty)
    {
        for (TFieldIterator<FProperty> It(DataTable->GetRowStruct()); It; ++It)
        {
            FProperty* Candidate = *It;
            const FString InternalName = Candidate->GetName();
            const FString DisplayName = Candidate->GetDisplayNameText().ToString();

            if (InternalName.Equals(RequestedName, ESearchCase::IgnoreCase)
                || DisplayName.Equals(RequestedName, ESearchCase::IgnoreCase)
                || InternalName.StartsWith(RequestedName + TEXT("_"), ESearchCase::IgnoreCase))
            {
                ColumnProperty = Candidate;
                break;
            }
        }
    }

    if (!ColumnProperty)
    {
        UE_LOG(LogDataTableSort, Warning, TEXT("Column '%s' was not found in '%s'."), *RequestedName, *DataTable->GetName());
        return {};
    }

    const bool bIsNumber = CastField<FNumericProperty>(ColumnProperty) != nullptr
        || CastField<FEnumProperty>(ColumnProperty) != nullptr
        || CastField<FBoolProperty>(ColumnProperty) != nullptr;

    const bool bIsText = CastField<FStrProperty>(ColumnProperty) != nullptr
        || CastField<FNameProperty>(ColumnProperty) != nullptr
        || CastField<FTextProperty>(ColumnProperty) != nullptr;

    if (!bIsNumber && !bIsText)
    {
        UE_LOG(LogDataTableSort, Warning, TEXT("Unsupported type for column '%s' in '%s'."), *RequestedName, *DataTable->GetName());
        return {};
    }

    const auto ReadNumber = [ColumnProperty](const uint8* RowData) -> double
    {
        const uint8* Value = ColumnProperty->ContainerPtrToValuePtr<uint8>(RowData);

        if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(ColumnProperty))
        {
            return static_cast<double>(EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Value));
        }

        if (const FNumericProperty* NumericProperty = CastField<FNumericProperty>(ColumnProperty))
        {
            if (NumericProperty->IsFloatingPoint())
            {
                return NumericProperty->GetFloatingPointPropertyValue(Value);
            }

            if (CastField<FByteProperty>(ColumnProperty)
                || CastField<FUInt16Property>(ColumnProperty)
                || CastField<FUInt32Property>(ColumnProperty)
                || CastField<FUInt64Property>(ColumnProperty))
            {
                return static_cast<double>(NumericProperty->GetUnsignedIntPropertyValue(Value));
            }

            return static_cast<double>(NumericProperty->GetSignedIntPropertyValue(Value));
        }

        if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(ColumnProperty))
        {
            return BoolProperty->GetPropertyValue(Value) ? 1.0 : 0.0;
        }

        return 0.0;
    };

    const auto ReadText = [ColumnProperty](const uint8* RowData) -> FString
    {
        const uint8* Value = ColumnProperty->ContainerPtrToValuePtr<uint8>(RowData);

        if (const FStrProperty* StringProperty = CastField<FStrProperty>(ColumnProperty))
        {
            return StringProperty->GetPropertyValue(Value);
        }

        if (const FNameProperty* NameProperty = CastField<FNameProperty>(ColumnProperty))
        {
            return NameProperty->GetPropertyValue(Value).ToString();
        }

        if (const FTextProperty* TextProperty = CastField<FTextProperty>(ColumnProperty))
        {
            return TextProperty->GetPropertyValue(Value).ToString();
        }

        return {};
    };

    TArray<FName> RowNames = DataTable->GetRowNames();
    const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();

    RowNames.Sort([&](const FName& LeftName, const FName& RightName)
    {
        uint8* const* LeftRow = RowMap.Find(LeftName);
        uint8* const* RightRow = RowMap.Find(RightName);

        if (!LeftRow || !RightRow || !*LeftRow || !*RightRow)
        {
            return LeftName.ToString().Compare(RightName.ToString(), ESearchCase::IgnoreCase) < 0;
        }

        int32 Comparison = 0;

        if (bIsNumber)
        {
            const double Left = ReadNumber(*LeftRow);
            const double Right = ReadNumber(*RightRow);
            Comparison = Left < Right ? -1 : (Left > Right ? 1 : 0);
        }
        else
        {
            Comparison = ReadText(*LeftRow).Compare(ReadText(*RightRow), ESearchCase::IgnoreCase);
        }

        if (Comparison == 0)
        {
            return LeftName.ToString().Compare(RightName.ToString(), ESearchCase::IgnoreCase) < 0;
        }

        return bDescending ? Comparison > 0 : Comparison < 0;
    });

    return RowNames;
}
