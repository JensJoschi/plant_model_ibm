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
# this file contains some code to produce example input files. Some of it might be outdated and wrong. 



# write an example json file to the current directory
#key is a string of form "(x1, y1, z1)", value is a random integer/a map of values or a random double
#there are nxy *nxy values. With nxy = 100, the file starts with "(0,0,0)" and ends with "(99,99,0)"

#create keys
nxy = 50
x <- 0: (nxy-1)
y <- 0: (nxy-1)
x <- rep(x, nxy)
y <- rep(y, each = nxy)
xy <- paste0("(", x, ",", y, ",", 0, ")")


#create values
#"(x1,y1,z1)": {"texture": "loamysand", "structure": "low"},
structure<-rep("low", nxy*nxy)
texturelist = c("Sand", "LoamySand", "SandyLoam", "SandyClayLoam", "Loam")
#the first 50 are sand, the next 50 are clay, the next 50 are loam, the next 50 are sandy loam, the next 50 are sandy clay loam
texture <- rep(texturelist, each = nxy*nxy/5)


json_list <- data.frame(key = NA, value = NA)
for (i in 1:(nxy*nxy)) {
  json_list[i,] <- c(xy[i], paste0(texture[i], structure[i]))
}
write("{", "soilclassexample.json")
for (i in 1:(nxy*nxy)) {
  write(paste0('"', json_list[i,1], '": "' ,json_list[i,2], '",') ,  "soilclassexample.json", append=T)
}
write ("}", "soilclassexample.json", append=T)


#write a sand file
json_list <- data.frame(xy = NA, random = NA)
for (i in 1:(nxy*nxy)) {
  json_list[i,] <- c(xy[i], 100-random_ints[i])
}
write("{", "sand_50.json")
for (i in 1:(nxy*nxy)) {
  write(paste0('"', json_list[i,1], '":' ,json_list[i,2], ",") ,  "sand_50.json", append=T)
}
write ("}", "sand_50.json", append=T)


#create a json file.
#Keys: "(x,y,z)", values: x in range 0,49, y in range 0,49, z always 0. total: 2500 keys
#values: jsons of form:
#{ "SHADING": a, "DEPTH": b}  where a is a random float 0-1, b is a random int 0-100

nxy = 50
#make a list of strings of form "(x, y, z)"
x <- 0: (nxy-1)
y <- 0: (nxy-1)
x <- rep(x, nxy)
y <- rep(y, each = nxy)
xy <- paste0("(", x, ",", y, ",", 0, ")")
shading <- sample(seq(0,1,0.001),nxy*nxy, replace = TRUE)
depth <- sample(0:100, nxy*nxy, replace = TRUE)
json_list <- data.frame(keys = NA, shading = NA, depth = NA)
for (i in 1:(nxy*nxy)) {
  json_list[i,] <- c(xy[i], shading[i], depth[i])
}
write("{", "json_example.json")
for (i in 1:(nxy*nxy)) {
  write(paste0('"', json_list[i,1], '":' ,"{ \"SHADING\": ", json_list[i,2], ", \"DEPTH\": ", json_list[i,3], "},") ,  "json_example.json", append=T)
}
write ("}", "json_example.json", append=T)

#create random soils based on transition matrix entries
tm <- read.table( "stm.csv", sep = ";", header= T)
tm<-tm[,-1]
soillist<-colnames(tm)
nxy = 50

#make a list of strings of form "(x, y, z)"
x <- 0: (nxy-1)
y <- 0: (nxy-1)
x <- rep(x, nxy)
y <- rep(y, each = nxy)
xy <- paste0("(", x, ",", y, ",", 0, ")")
soilclass <- sample(soillist[1:20],nxy*nxy, replace = TRUE)
write("{", "randomsoils.json")
json_list <- data.frame(xy = NA, random = NA)
for (i in 1:(nxy*nxy)) {
  json_list[i,] <- c(xy[i], soilclass[i])
}

for (i in 1:(nxy*nxy)) {
  write(paste0('"', json_list[i,1], '":\"' ,json_list[i,2], "\",") ,  "randomsoils.json", append=T)
}
write ("}", "randomsoils.json", append=T)
