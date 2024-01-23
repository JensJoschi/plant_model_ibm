#*------------------------------------------------------------------------------
#Copyright (C) 2023 - present Jens Joschinski

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
# some notes and scribbles to fix some typos in an existing PFG.csv file, and convert into a more or less usable json. 


orig <- read.table("rscripts/output.csv", sep = ",", header=T)
orig<-orig[!orig$SoilTol == " ",]
orig$PotentialFecundity<-1
for (i in 1:nrow(orig)){
  orig$PoolL[i]<- paste(c(0,orig$PoolL[i] ), collapse = ",")
  orig$LightTolerance[i]<- paste0("[[true, true, true],[",
         paste(strsplit(orig$LightTolerance[i], ",")[[1]][1:3], collapse = ","), "],[",
         paste(strsplit(orig$LightTolerance[i], ",")[[1]][1:3], collapse = ","), "],[",
         paste(strsplit(orig$LightTolerance[i], ",")[[1]][1:3], collapse = ","), "]]")
}
names(orig)[5]<-"Max_Abund" #capitalization typo


write.table(orig, file ="output.csv", sep  =",", row.names = F)

writePFGs <- function(infile, outfile){
  #this function writes a table with PFG attributes and puts them into a json file
  #the keys of the json file are clearly defined and hardcoded in the model, so they cannot be changed
  #the head of the infile needs to correspond exactly to those keys, otherwise an error occurs
  #infile: file of csv format; outfile: the PFG definitions (json) file

    orig <- read.table(infile, sep = ",", header=T)
    write( "{\n",outfile )
    pfg<-orig[1,]
    for (i in 1:nrow(orig)){
     pfg<-orig[i,]
     write(paste0("\t\"", pfg$Name, "\":{\n"),outfile, append = T)
     write(paste0("\t\t\"Name\": \"", pfg$Name, "\",\n"),outfile, append = T)
     write(paste0("\t\t\"Maturation_time\": ", pfg$Maturation_time, ",\n"),outfile, append = T)
     write(paste0("\t\t\"Life_Span\": ", pfg$Life_Span, ",\n"), outfile, append = T)
     write(paste0("\t\t\"Max_Abund\": \"", pfg$Max_Abund, "\",\n"), outfile, append = T)
     write(paste0("\t\t\"ImmSize\": ", pfg$ImmSize, ",\n"), outfile, append = T)
#warning: this should be a value between 0 - 1, in contrast to the original RFate. If not, divide by 100
     write(paste0("\t\t\"MaxStratum\": ", pfg$MaxStratum, ",\n"), outfile, append = T)
     write(paste0("\t\t\"StrataChangeAge\": [", pfg$StrataChangeAge, "],\n"), outfile, append = T)
     write(paste0("\t\t\"PoolL\": [", pfg$PoolL, "],\n"), outfile, append = T)
#     if (pfg$InnateDorm) {write(paste0("\t\t\"InnateDorm\": true,\n"), outfile, append = T)}
#     else{write(paste0("\t\t\"InnateDorm\": false,\n"), outfile, append = T)}
     write(paste0("\t\t\"PotentialFecundity\": ", pfg$PotentialFecundity, ",\n"), outfile, append = T)
     write(paste0("\t\t\"LightShadeFactor\": ", pfg$LightShadeFactor, ",\n"), outfile, append = T)
     write(paste0("\t\t\"LightActiveGerm\": [", pfg$LightActiveGerm, "],\n"), outfile, append = T)
     write(paste0("\t\t\"LightTolerance\": ", pfg$LightTolerance, ",\n"), outfile, append = T)
 #    Dispersal here
     write("\t\t\"SoilTol\": [", outfile, append = T)
     t<-strsplit(pfg$SoilTol, " ")[[1]]
     for (j in 1:length(t)){ write(paste0("\"", t[j],"\""), outfile, append = T)
       if (j != length(t)) write(",", outfile, append = T) else write ( "],", outfile, append = T)
     }
     write(paste0("\t\t\"DepthReq\": ", pfg$DepthReq, "\n"), outfile, append = T)
     if(nrow(orig) != i){ write( "},\n", outfile, append = T)} else {write( "}",outfile, append = T)}
    }
    write( "\n}",outfile, append = T)
}
