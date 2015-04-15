# CHESTER #
<p align="center"><img src="/logo.png" 
alt="CHESTER" width="300" height="200" border="0" /></p>
CHESTER is an alignment-free tool to compute and visualise uniqueness maps.
The target sequences should be aligned (FASTA), while the references can be non-aligned, such as those outputed directly from NGS plataforms (FASTQ). CHESTER has probabilistic way to detect the presence of large k-mer sizes (up to 31), namely using bloom filters.

## INSTALLATION ##

Cmake is needed for installation (http://www.cmake.org/). You can download it directly from http://www.cmake.org/cmake/resources/software.html or use an appropriate packet manager. In the following instructions we show the procedure to install, compile and run CHESTER:

### STEP 1

Download, install and resolve conflicts.

#### Linux 
<pre>
sudo apt-get install cmake
wget https://github.com/pratas/chester/archive/master.zip
unzip master.zip
cd chester-master
cmake .
make
</pre>

Alternatively, you can install (without cmake and only for linux) using

<pre>
wget https://github.com/pratas/chester/archive/master.zip
unzip master.zip
cd chester-master
mv Makefile.linux Makefile
make
</pre>

#### OS X
Install brew:
<pre>
ruby -e "$(curl -fsSL https://raw.github.com/Homebrew/homebrew/go/install)"
</pre>
only if you do not have it. After type:
<pre>
brew install cmake
brew install wget
brew install gcc48
wget https://github.com/pratas/chester/archive/master.zip
unzip master.zip
cd chester-master
cmake .
make
</pre>
With some versions you might need to create a link to cc or gcc (after the *brew install gcc48* command), namely
<pre>
sudo mv /usr/bin/gcc /usr/bin/gcc-old   # gcc backup
sudo mv /usr/bin/cc /usr/bin/cc-old     # cc backup
sudo ln -s /usr/bin/gcc-4.8 /usr/bin/gcc
sudo ln -s /usr/bin/gcc-4.8 /usr/bin/cc
</pre>
In some versions, the gcc48 is installed over /usr/local/bin, therefore you might need to substitute the last two commands by the following two:
<pre>
sudo ln -s /usr/local/bin/gcc-4.8 /usr/bin/gcc
sudo ln -s /usr/local/bin/gcc-4.8 /usr/bin/cc
</pre>

#### Windows

In windows use cygwin (https://www.cygwin.com/) and make sure that it is included in the installation: cmake, make, zcat, unzip, wget, tr, grep (and any dependencies). If you install the complete cygwin packet then all these will be installed. After, all steps will be the same as in Linux.

## EXECUTION

### Run CHESTER

Run CHESTER:

<pre>
./CHESTER -v -k 30 -i -t 0.5 -s 6099999999 File1.fastq:File2.fastq:File3.fasta FileA.fasta:FileB.fasta
</pre>

## PARAMETERS

To see the possible options type
<pre>
./CHESTER
</pre>
or
<pre>
./CHESTER -h
</pre>

These will print the following options:
<pre>
<p>
Usage: CHESTER &#60OPTIONS&#62... [FILE]:&#60...&#62 [FILE]:&#60...&#62
CHESTER is a fast tool to compute uniqueness maps.   
                                                     
  -v                       verbose mode,             
  -a                       about CHESTER,            
  -t &#60value&#62               threshold [0.0;1.0],      
  -w &#60value&#62               window size,              
  -u &#60value&#62               sub-sampling,             
  -n &#60value&#62               bloom hashes number,      
  -s &#60value&#62               bloom size,               
  -i                       use inversions,           
  -k &#60value&#62               k-mer size,               
                                                     
  [rFile1]:&#60rFile2&#62:&#60...&#62  reference file(s),   
  [tFile1]:&#60tFile2&#62:&#60...&#62  target file(s).           

The reference files may be FASTA, FASTQ or DNA SEQ,
while the target files may be FASTA OR DNA SEQ.                                     Report bugs to &#60{pratas,ap,pjf}@ua.pt&#62. 
</pre>

## CITATION ##

On using this software/method please cite:

PAPER: comming soon.
DOI: comming soon.

## ISSUES ##

For any issue let us know at [issues link](https://github.com/pratas/chester/issues).

## LICENSE ##

GPL v2.

For more information:
<pre>http://www.gnu.org/licenses/gpl-2.0.html</pre>

                                                    

