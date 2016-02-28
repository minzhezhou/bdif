#include "mpi.h"
#include <iostream>
#include <string>
using namespace std;
long long const FILESIZE = 100;

int main(int argc, char** argv) {
int rank, size, bufsize, nints;
    long long file_sz;
    MPI_File fh;
    MPI_Status status;
    MPI_Offset fsz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    string fname = "testfile";
   
    //int MPI_File_open(MPI_Comm comm, const char *filename,
    //              int amode, MPI_Info info,
    //                        MPI_File *fh)
    int ret = MPI_File_open(MPI_COMM_WORLD, fname.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    cout << "mpi_fopen ret:" << ret << endl;

    //int MPI_File_get_size(MPI_File fh, MPI_Offset *size)
    ret = MPI_File_get_size(fh, &fsz);
    cout << "mpi_file_sz ret:" << ret << " file size:" << fsz << endl;
    int local_sz = fsz / size;
    int read_cnt = rank==size-1 ? fsz - (size-1)*local_sz : local_sz;
    cout << "rank is:" << rank << " size is:" << size << " read from " << rank * local_sz 
        << " to " << rank * local_sz + read_cnt << endl;
    //int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset,
    //    void *buf, int count, MPI_Datatype datatype,
    //        MPI_Status *status)
    char buf[10000];
    MPI_File_read_at_all(fh, local_sz*rank, buf, read_cnt, MPI_BYTE, &status);
    cout << "rank:" << rank << " buf content|" ;
    for (int i = 0; i < read_cnt; i++) {
        cout << buf[i];
    }
    cout << "|" << endl;

    //int MPI_Allgather(const void *sendbuf, int  sendcount,
    //     MPI_Datatype sendtype, void *recvbuf, int recvcount,
    //          MPI_Datatype recvtype, MPI_Comm comm)
    
    int local_write_sz = read_cnt / 2;
    int *allsz = (int*)malloc(sizeof(int) * size);
    MPI_Allgather(&local_write_sz, 1, MPI_INT, allsz, 1, MPI_INT, MPI_COMM_WORLD);
    cout << "rank:" << rank << "is alive" << endl;
    for (int i = 1; i < size; i++) {
        allsz[i] += allsz[i-1];
    }
    cout << "\n\n\nrank:" << rank << " write from " << allsz[rank] << " to " 
        << allsz[rank] + local_write_sz << endl; 
    MPI_Finalize();
    return 0;
}
