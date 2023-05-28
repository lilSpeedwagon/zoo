# Data file transactions

The data storage mechanism requires not only data persistence but also data durability. The database must be robust even if something went wrong during filesystem I/O operations. For example, in case of a power outage while writing data on the disk we must be able to restore the last valid state of data. To do so we need filesystem transactions.

The transaction is a data update process that cannot be observed in the intermediate state. Transaction can be completed, or rollbacked. There is no "middle" state. To do so, we can apply a backup strategy. Before making any changes to a file, we can create a copy of it. All changes will be applied to the copy. If the transaction is completed successfully - we replace the original file with the copy. If something went wrong during a transaction we just remove the copy. If the writing program is down during a transaction, we won't lose the valid data state.

## Algorithm

Two versions of the algorithm could be used depending on the required degree of robustness.

### Naive version

The first version is more naive, but also more performant. It creates a copy of the changeable file and performs all I/O operations with it. If everything is fine, the original file will be replaced with the copy.

1. Check if there is a copy `<filename>.new_version` file. If so - remove it.
2. Make a copy of the writable file with the name `<filename>.new_version`.
3. Make changes in the new file.
4. Rename the copy to the original name (with replacement.

### Strict version

This version adds a second write phase by making a backup of an original file. It may be helpful in case of failure during replacing the original file with the copy. This algorithm requires more I/O operations with the filesystem (more copying, and removal).

1. Check if there is an old `<filename>.backup` file. If so - replace the original file with the backup.
2. Check if there is a copy `<filename>.new_version` file. If so - remove it.
3. Make a copy of the writable file with name `<filename>.new_version`.
4. Make changes in the new file.
5. Rename the original file to `<filename>.backup`.
6. Rename the copy to the original name.
7. Remove the backup.
