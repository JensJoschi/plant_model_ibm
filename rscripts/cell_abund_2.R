#*------------------------------------------------------------------------------
# Copyright (C)  2022 - present  Studio Animal-Aided Design

#This file is part of the ECOLOPES PLANT MODEL.

#ECOLOPES PLANT MODEL is free software: you can redistribute it and/or modify 
#it under the terms of the GNU General Public License as published by the 
#Free Software Foundation, either version 3 of the License, or (at your option) any later version.

#ECOLOPES PLANT MODEL is distributed in the hope that it will be useful, 
#but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
#or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

#You should have received a copy of the GNU General Public License along with ECOLOPES PLANT MODEL. 
#If not, see <https://www.gnu.org/licenses/>. */

# --------------------------------------------------------------------------
# /* ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
# * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
# * For contributions to this particular file, see section "Authors and contributors".*/
# --------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Authors and contributors to this file:
# Jens Joschinski
# --------------------------------------------------------------------------

# internal use only, this file has little meaning for the user
#simple script to analyse model output. Might be outdated and wrong, please adapt while using.


library(terra)
library(jsonlite)
library(tidyverse)

#constants
t = 50
wd = "/Users/jensjoschinski/demos/out-run1/"

map_pfg <- function(pfg, t, wd = "test-RESULTS/"){
  #this function reads the abundance of a pfg at time point t and returns a map of it.
  filename <- paste0(wd, "t_", t-1, pfg, "_plantbiomass.json")
  data<-read_json(filename) %>% as_tibble() %>% t()
  x <- rep(NA, nrow(data))
  y <- rep(NA,nrow(data))
  for (i in 1:nrow(data)){
    stripped <- rownames(data)[i]
    stripped <- substr(stripped, 2, nchar(stripped)-1)
    x[i]<-str_split(stripped, ",")[[1]][1]
    y[i]<-str_split(stripped, ",")[[1]][2]
  }
  d<-data.frame(as.numeric(x),as.numeric(y),val = data[,1])
  d<-rast(d, type="xyz")
  return(d)
}

plot_summed_abund_pfg<- function(pfg, t, outfile, wd = "test-RESULTS/"){
  #this function reads the abundances of a pfg from the plantbiomass.json file in wd,
  #and then creates a plot of abundance over time (abundance is summed across the site)
  tx<-list()
  for ( i in 1:t){
    tx[[i]] <- map_pfg(pfg, i,wd = wd)
  }
  n<-data.frame(val = rep(NA,t))
  for (i in 1:t){
    n$val[i]<-sum(values(tx[[i]]))
  }
  if (outfile != "") jpeg(outfile)
  plot(n$val,type= "l", main = paste0("Change of PFG ", pfg, " abundance over time"),xlab = "time",ylab = "summed abundance")
  if (outfile != "") dev.off()
}

plot_summed_abund_pfg("epiphytes_3", t, "", wd)


data<-read_json(paste0(wd,"plantbiomass.json"))

biom_cell<-function(cell,tmax, data){
  #function to get the total biomass in a cell
  res<-data.frame(x = 1:tmax, y = NA)
  for (i in 1:(tmax)){
    res$x[i] <- i
    res$y[i] <- data[[cell]][[i]]%>%as_tibble()%>%t()%>%sum()
  }
  return(res)
}

biom_table<- function(pbm, t){
  #funtion to get a table with total biomasses in each cell (columns) at all times (rows)
  cell_list<-names(pbm)
  rx<-biom_cell(cell_list[1],t, pbm)
  names(rx)<-c("t", "cell1")
  for(x in 2:(length(cell_list)+1)){
    r<-data.frame(biom_cell(cell_list[x], t, pbm)$y)
    names(r)<-paste0("cell",x)
    rx<-cbind(rx,r)
  }
  return(rx)
}

tab<-biom_table(data,t)

plot_summed_abund<-function(data, rows, outfile){
 if (outfile != "") jpeg(outfile)
 plot(NA, type= "l",ylim = c (0,max(data[,rows])),xlim = c(0,max(data$t)),
      main = paste0("change in total plant biomass over time"), xlab = "time", ylab = "cell Abundance")
  for (i in rows){
    lines(data$t, data[,i], col = i)
  }
 legend("topleft", legend = paste0("Cell ", rows), col = rows, lty= 1 )
  if (outfile != "") dev.off()
}

plot_summed_abund(tab, 11:13, "")

plot_summed_abund.total<-function(data, outfile){
  if (outfile != "" ) jpeg("total_biomass.jpg")
  plot(rowSums(data[,-1]), type= "l",ylim = c (0,max(rowSums(data[,-1]))),
       xlim = c(0,max(data$t)), main = "change in total abund over time",
       xlab = "time", ylab = "total PFG Abundance")
  if (outfile != "" ) dev.off()
}
plot_summed_abund.total(tab,"")

pres_abs<-function(cell, t, data){
  #provides a table indicating which PFG is available in a cell at time t
  r<-data[cell][[1]][[t]]%>%as_tibble%>%t()>0
  return(r)
}
pres_abs(4,t, data)

myresult<-matrix(NA, nrow = length(data), ncol = t)
for (cell in 1:length(data)){
  for (t_l in 1:t){
    myresult[cell,t_l] <- sum(pres_abs(names(data)[cell],t_l,data))
  }
}

plot(myresult[11,], type = "l", ylim = range(myresult[11,], na.rm=T),
     main = "Species richness change over time in cell 11")


jpeg("richness.jpg")
plot(colMeans(myresult), type ="l", main = "Change of site-wide mean FG richness over time",
     xlab = "Time", ylab = "PFG richness")
dev.off()


shad<-read_json("0-INPUT/json_example.json")
shading <- data.frame(cell = names(shad), val =NA)
for (i in 1:nrow(shading)){
  shading$val[i]<- shad[[i]]$SHADING
}

cell_list<-names(data)
r<-data.frame(val = rep(NA,2500))
for(x in 1:2500){
  r$val[x]<-mean(biom_cell(cell_list[x], 9)$y)
}


soil<-read_json("0-INPUT/randomsoils.json")
s<-data.frame(cell = names(soil), val=NA)
for (i in 1:nrow(s)){
  s$val[i]<- soil[[i]]
}
jpeg("soil_type.jpg")
plot(r$val~as.factor(s$val), main = "PFG abundance by soil type")
dev.off()

#subset plant abundance and shading by soil class
sub<-r[s$val=="Cl_h_2",]
sub2<-shading[s$val=="Cl_h_2",]
jpeg("abund_shad_oneSoil.jpg")
plot(sub~sub2$val, main = "Relationship of PFG abundance with shading,\n for soil Cl_h_1")
dev.off()
#make mean over time!
