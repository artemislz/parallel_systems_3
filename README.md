## Οδηγίες εκτέλεσης 3.1:
### Σειριακό:
```bash
make 3_1_serial     
./bin/3_1_serial <generations> <matrix_dim>
```
### Παράλληλο:
```bash
make 3_1    
mpiexec -f <machines_file> -n <proc_num> ./bin/3_1 <generations> <matrix_dim>
```
## Οδηγίες εκτέλεσης 3.2:
### Σειριακό:
```bash
make 3_2_serial     
./bin/3_2_serial <matrix_dim>
```
### Παράλληλο:
```bash
make 3_2
mpiexec -f <machines_file> -n <proc_num> ./bin/3_2 <matrix_dim>
```

