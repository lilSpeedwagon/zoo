# Document DB storage

Document is a pair of Document Payload and Document Info. Document Info is it's meta-information: id, time of creation, name, etc. Document Info also contains information about location of payload in the filesystem. In other words, Document Info also used as an index value. Document Info is stored to to `meta.ddb` file as an indexed structure. Payloads are stored in a number of files and index helps to find a specific payload among these files. To store payload location we use Document Position structure contains file name and position in this file.

Payload is stored to files with append-only semantic. It helps avoid rewriting of the entire file. So an updated payload will be written to the end of a file and an old value becomes outdated. This payload should be marked as outdated with some bit flag. When payload file reaches a specific size, we assume that this file is full and do not store there payloads anymore. With more updates more payloads become outdated. When all of the file payloads are outdated - we can delete this file. After updating a payload in the end of file, we also must update Document Position according to new file position.

### Payload create steps

1. Find a data file to store payload
    a. Iterate over existing file to find one with enough available size
    b. Create a new file if no files match the previous criteria
2. Set first bit before payload to 1 to mark it as an active
3. Append a payload data to the end of the file
4. Save file name and position in file to Document Position structure

### Payload update steps

1. Find a data file to store payload
    a. Iterate over existing file to find one with enough available size
    b. Create a new file if no files match the previous criteria
2. Set first bit before payload to 1 to mark it as an active
3. Append a payload data to the end of the file
4. Set active bit of the old payload to 0
5. Update Document Position structure
6. Check the old paload location whether it is needed to cleanup entrire file
    a. If the old and the new files are the same - do nothing
    b. Iterate over file's payloads and check active bits
    c. If all payloads are outdated - delete the file

### Payload file structure

```
| is_active (1 bit) | size (31 bit) | payload (\<size> bytes) | ... |
```
