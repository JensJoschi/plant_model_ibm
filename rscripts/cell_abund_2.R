#*------------------------------------------------------------------------------
# Copyright (C)  2022 - present  Studio Animal-Aided Design

#This file is part of the INDIVIDUAL-BASED PLANT MODEL.

#INDIVIDUAL-BASED PLANT MODEL is free software: you can redistribute it and/or modify 
#it under the terms of the GNU General Public License as published by the 
#Free Software Foundation, either version 3 of the License, or (at your option) any later version.

#INDIVIDUAL-BASED PLANT MODEL is distributed in the hope that it will be useful, 
#but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
#or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

#You should have received a copy of the GNU General Public License along with INDIVIDUAL-BASED PLANT MODEL. 
#If not, see <https://www.gnu.org/licenses/>. */

# --------------------------------------------------------------------------
# INDIVIDUAL-BASED PLANT MODEL is derived from the ECOLOPES PLANT MODEL, Copyright (C) 2022-present Studio Animal-Aided Design.

# /* ECOLOPES PLANT MODEL is derived, modified and extended from FATE, https://github.com/leca-dev/RFate.git/ 
# * Copyright (C) 2021 Isabelle Boulangeat, Damien Georges, Maya Guéguen, Wilfried Thuiller 
# * For contributions to this particular file, see section "Authors and contributors".*/
# --------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Authors and contributors to this file:
# Jens Joschinski (plant model)
# Jens Joschinski (IBM): adapt to new model
# --------------------------------------------------------------------------

# internal use only, this file has little meaning for the user
#simple script to analyse model output.

library(jsonlite)
library(tidyverse)
library(rgl)

setwd("/Users/jensjoschinski/plant-model-ibm/tests/")

readData<-function (filename){
  #json has format: {"(0, 0, 0)": {"PFG1": 14.4, "PFG2": 23.0}, "(0, 0, 1)": ...}
  # read into table with x, y, z, "PFG1", "PFG2", ...
  temporary<-fromJSON(filename)
  temp_df <- lapply(temporary, as.data.frame)
  data <- do.call(rbind, temp_df)
  data$x <- NA
  data$y <- NA
  data$z <- NA
  for (i in 1:nrow(data)){
    name <- rownames(data)[i]
    name <- gsub("\\(", "", name)
    name <- gsub("\\)", "", name)
    split_name <- strsplit(name, ", ")
    data$x[i] <- as.numeric(split_name[[1]][1])
    data$y[i] <- as.numeric(split_name[[1]][2])
    data$z[i] <- as.numeric(split_name[[1]][3])
  }
  return (data)
}

plotOne <- function (data, name, maxCol){
  colfunc<- colorRampPalette(c("white", "green"))
  cols<-colfunc(maxCol)
  plot3d(data$x,data$y,data$z, size = 1, type = "s", decorate= F, col = cols[round(data[, name])+1])
}


wd = "/Users/jensjoschinski/plant-model-ibm/tests/"
t1<- readData(paste0(wd, "RESULTS_building2/biomass_1.json"))
t2<- readData(paste0(wd, "RESULTS_building2/biomass_2.json"))
t4<- readData(paste0(wd, "RESULTS_building2/biomass_4.json"))
t10<- readData(paste0(wd, "RESULTS_building2/biomass_10.json"))
t49<- readData(paste0(wd, "RESULTS_building2/biomass_49.json"))
plotOne(t1, "ArtemisiaVulgaris", 120)
rgl.snapshot("Art_1.jpg")
plotOne(t2, "ArtemisiaVulgaris", 120)
rgl.snapshot("Art_2.jpg")
plotOne(t4, "ArtemisiaVulgaris", 120)
rgl.snapshot("Art_4.jpg")
plotOne(t10, "ArtemisiaVulgaris", 120)
rgl.snapshot("Art_10.jpg")
plotOne(t49, "PapaverRhoeas", 120)
rgl.snapshot("Art_49.jpg")

#BellisPerennis
plotOne(t1, "BellisPerennis", 120)
rgl.snapshot("Bel_1.jpg")
plotOne(t2, "BellisPerennis", 120)
rgl.snapshot("Bel_2.jpg")
plotOne(t4, "BellisPerennis", 120)
rgl.snapshot("Bel_4.jpg")
plotOne(t10, "BellisPerennis", 120)
rgl.snapshot("Bel_10.jpg")
plotOne(t49, "BellisPerennis", 120)
rgl.snapshot("Bel_49.jpg")


#sum#
t1s<- readData(paste0(wd, "RESULTS_building2/summed_biomass_1.json"))
t2s<- readData(paste0(wd, "RESULTS_building2/summed_biomass_2.json"))
t4s<- readData(paste0(wd, "RESULTS_building2/summed_biomass_4.json"))
t10s<- readData(paste0(wd, "RESULTS_building2/summed_biomass_10.json"))
t49s<- readData(paste0(wd, "RESULTS_building2/summed_biomass_49.json"))
plotOne(t1s, "X[[i]]", 200)
rgl.snapshot("summed_1.jpg")
plotOne(t2s, "X[[i]]", 200)
rgl.snapshot("summed_2.jpg")
plotOne(t4s, "X[[i]]", 200)
rgl.snapshot("summed_4.jpg")
plotOne(t10s, "X[[i]]", 200)
rgl.snapshot("summed_10.jpg")
plotOne(t49s, "X[[i]]", 200)
rgl.snapshot("summed_49.jpg")

t1SA<- readData(paste0(wd, "RESULTS_building2/summed_abundance_1.json"))
t2SA<- readData(paste0(wd, "RESULTS_building2/summed_abundance_2.json"))
t4SA<- readData(paste0(wd, "RESULTS_building2/summed_abundance_4.json"))
t10SA<- readData(paste0(wd, "RESULTS_building2/summed_abundance_10.json"))
t49SA<- readData(paste0(wd, "RESULTS_building2/summed_abundance_49.json"))
plotOne(t1SA, "X[[i]]", 40)
rgl.snapshot("summedAbund_1.jpg")
plotOne(t2SA, "X[[i]]", 40)
rgl.snapshot("summedAbund_2.jpg")
plotOne(t4SA, "X[[i]]", 40)
rgl.snapshot("summedAbund_4.jpg")
plotOne(t10SA, "X[[i]]", 40)
rgl.snapshot("summedAbund_10.jpg")
plotOne(t49SA, "X[[i]]", 40)
rgl.snapshot("summedAbund_49.jpg")

ab10<- readData(paste0(wd, "RESULTS_building2/abundance_10.json"))
plotOne(ab10, "BellisPerennis", 20)
rgl.snapshot("abBellis.jpg")
plotOne(ab10, "ArtemisiaVulgaris", 20)
rgl.snapshot("abArt.jpg")


# area
t1A<- readData(paste0(wd, "RESULTS_building2/area_1.json"))
names(t1A)[1]<- "area" 
t2A<- readData(paste0(wd, "RESULTS_building2/area_2.json"))
names(t2A)[1]<- "area" 
t4A<- readData(paste0(wd, "RESULTS_building2/area_4.json"))
names(t4A)[1]<- "area" 
t5A<- readData(paste0(wd, "RESULTS_building2/area_5.json"))
names(t5A)[1]<- "area" 
t6A<- readData(paste0(wd, "RESULTS_building2/area_6.json"))
names(t6A)[1]<- "area" 
t7A<- readData(paste0(wd, "RESULTS_building2/area_7.json"))
names(t7A)[1]<- "area" 
t8A<- readData(paste0(wd, "RESULTS_building2/area_8.json"))
names(t8A)[1]<- "area" 
t9A<- readData(paste0(wd, "RESULTS_building2/area_9.json"))
names(t9A)[1]<- "area" 
t10A<- readData(paste0(wd, "RESULTS_building2/area_10.json"))
names(t10A)[1]<- "area" 
t20A<- readData(paste0(wd, "RESULTS_building2/area_20.json"))
names(t20A)[1]<- "area" 
t21A<- readData(paste0(wd, "RESULTS_building2/area_21.json"))
names(t21A)[1]<- "area" 
t22A<- readData(paste0(wd, "RESULTS_building2/area_22.json"))
names(t22A)[1]<- "area" 
t23A<- readData(paste0(wd, "RESULTS_building2/area_23.json"))
names(t23A)[1]<- "area" 
t24A<- readData(paste0(wd, "RESULTS_building2/area_24.json"))
names(t24A)[1]<- "area" 
t25A<- readData(paste0(wd, "RESULTS_building2/area_25.json"))
names(t25A)[1]<- "area" 
t49A<- readData(paste0(wd, "RESULTS_building2/area_49.json"))
names(t49A)[1]<- "area" 

plotOne(t1A, "area", 20)
plotOne(t2A, "area", 200)
plotOne(t4A, "area", 200)
plotOne(t5A, "area", 200)
plotOne(t6A, "area", 200)
plotOne(t7A, "area", 200)
plotOne(t8A, "area", 200)
plotOne(t9A, "area", 200)
plotOne(t10A, "area", 200)
plotOne(t20A, "area", 400)
plotOne(t21A, "area", 400)
plotOne(t22A, "area", 400)
plotOne(t23A, "area", 400)
plotOne(t24A, "area", 400)
plotOne(t25A, "area", 400)


plotOne(t49A, "area", 315)
