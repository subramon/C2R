install.packages( "/home/subramon/my_Rserve/Rserve",
                 repo = NULL,
                 type = "source",
                 lib = "/home/subramon/")

# Warning in gzfile(file, "rb") :
#  cannot open compressed file '/WORK/my_mgcv/mgcv/Meta/package.rds', probable reason 'No such file or directory'
#Error in gzfile(file, "rb") : cannot open the connection
#Error: loading failed
#Execution halted
#ERROR: loading failed
#* removing '/WORK/my_mgcv/mgcv'
#Warning message:
#In install.packages("../../my_mgcv/mgcv_1.8-31.tar.gz", repo = NULL,  :
#  installation of package '../../my_mgcv/mgcv_1.8-31.tar.gz' had non-zero exit status

# When you call install.packages("…", lib = …, ….), remove lib as an option. I am curious to see what happens.

# That’s great. I suspect R may create a lock on the directory that contains the tar.gz source. It seemed you were using the same dir to put the final compiled library files as well. If you use a different location for the lib than the actual tar.gz source, I think it will work.
