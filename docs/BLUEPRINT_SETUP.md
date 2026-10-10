# Blueprint setup

1. Create a `Name Array` variable named `MonsterRows` in `WBP_Bestiar`.
2. Add **Event On Initialized** to the graph.
3. Add **Get Sorted Data Table Row Names**.
4. Set **Data Table** to `DT_Monsters`.
5. Set **Column Name** to `Threat`.
6. Enable **Descending** to show high-threat entries first.
7. Connect its **Return Value** to **Set MonsterRows**.
8. Page through `MonsterRows` using its array indexes, then call `ShowMonster(Row Name)` for the selected item.

To sort alphabetically by the monster's display name, set `Column Name` to the `Name` field in the row structure instead of the row ID.

The plugin leaves the original DataTable unchanged.
