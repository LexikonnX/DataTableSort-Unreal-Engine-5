# DataTableSort

![DataTable Sort icon](Resources/Icon128.png)

A lightweight **Unreal Engine 5 C++ plugin** that sorts DataTable row names by the values in any supported column, directly from Blueprints.

Instead of sorting by the row names themselves, sort by properties such as `Threat`, `Price`, `Level`, or `Name` without manually maintaining arrays or adding sorting logic to your widget.

## Features

- One Blueprint-pure node: **Get Sorted Data Table Row Names**
- Use **any DataTable** and specify the **column name**
- **Ascending** and **descending** sorting
- Numeric sorting for integers, floats, doubles, enums and booleans
- Case-insensitive sorting for strings, names and texts
- Supports user-defined Blueprint structs (including internal field names with generated suffixes)
- Consistent tie-breaking by row name (A–Z)
- No external dependencies

## Installation

1. [Download this repository](https://github.com/LexikonnX/DataTableSort-Unreal-Engine-5/releases/tag/latest).
2. Extract its contents into `<YourProject>/Plugins/DataTableSort/`. The `DataTableSort.uplugin` file must end up directly inside that folder.
3. Open your Unreal Engine project. Enable **DataTable Sort** under **Edit → Plugins**, if it is not already enabled.
4. If Unreal asks to compile the plugin, build the project with a compatible C++ toolchain (e.g. Visual Studio 2022 on Windows).
5. Restart Unreal Editor if requested.

The plugin is source-only: binaries are not included.

## Blueprint usage

Create a call to **Get Sorted Data Table Row Names**, with the following inputs:

| Input | Type | Description |
| --- | --- | --- |
| `Data Table` | DataTable Object Reference | DataTable you want to sort |
| `Column Name` | Name | Column/property to sort by, such as `Threat` |
| `Descending` | Boolean | `false` = lowest/earliest first; `true` = highest/latest first |

The node returns a `Name Array` of row IDs. Use it with **Get Data Table Row** to fetch data for each row.

### Example: monster bestiary

In your widget's **On Initialized** event:

```text
Event On Initialized
         ↓
Get Sorted Data Table Row Names
  Data Table  = DT_Monsters
  Column Name = Threat
  Descending  = true
         ↓
Set MonsterRows
```

A `Threat` column containing the values `1, 4, 2, 5` produces the row name order `5, 4, 2, 1` by threat value. Items with the same threat level are ordered by their row ID from A to Z.

![Blueprint](https://i.imgur.com/Higsh40.png)

### Other examples

| Column Name | Descending | Effect |
| --- | --- | --- |
| `Threat` | `true` | Most dangerous monsters first |
| `Price` | `false` | Cheapest products first |
| `Level` | `false` | Lowest level first |
| `Name` | `false` | Names alphabetically (A–Z) |

## Supported column types

- Integer types (including byte and unsigned integer)
- Float and Double
- Enum
- Boolean
- String, Name and Text

Array, struct, object and other complex fields are not supported. Unknown or unsupported columns return an empty array and log a warning. The node sorts **row IDs** according to the chosen field, without modifying the DataTable itself.

## Notes

- Use the DataTable **field/property name**, not a value stored in a row.
- For Blueprint-created structures, `Column Name = Threat` is accepted even when the generated C++ property name contains a suffix.
- Sorting is performed on demand; store the result in a variable instead of calling it on every frame.
- This plugin is designed for UE5 source builds and has been prepared against the UE 5.8 API; it has **not yet been compilation-tested in a live Unreal project**.

## License

MIT. See [LICENSE](LICENSE).
