#!/bin/bash


sudo apt install r-base r-base-dev -y

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E084DAB9  

sudo add-apt-repository ppa:marutter/rdev

sudo apt update  

sudo apt upgrade  

sudo su - -c "R -e \"install.packages('ggplot2', repos='http://cran.rstudio.com/')\""

sudo su - -c "R -e \"install.packages('dplyr', repos='http://cran.rstudio.com/')\""

sudo su - -c "R -e \"install.packages('DoE.base', repos='http://cran.rstudio.com/')\""

sudo su - -c "R -e \"install.packages('reshape', repos='http://cran.rstudio.com/')\""

sudo su - -c "R -e \"install.packages('readr', repos='http://cran.rstudio.com/')\""

sudo su - -c "R -e \"install.packages('tidyr', repos='http://cran.rstudio.com/')\""		