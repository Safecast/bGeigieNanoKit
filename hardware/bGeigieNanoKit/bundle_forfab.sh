#!/bin/bash

PROJECT="bGeigieNanoKit"
OUTDIR="tofab"


# Instructions for generating pdf outputs
# 
PDFS="${PROJECT}.pdf \
	${PROJECT}_brd.pdf \
	${PROJECT}_gerbers_bw.pdf \
	${PROJECT}_gerbers_color.pdf"

GERBERS="${PROJECT}.gpi \
	${PROJECT}.sst \
	${PROJECT}.smt \
	${PROJECT}.top \
	${PROJECT}.bot \
	${PROJECT}.smb \
	${PROJECT}.ssb \
	${PROJECT}.drd \
	${PROJECT}.dri"

ASMFILES="${PROJECT}.bom \
	${PROJECT}.asm \
	${PROJECT}.spt \
	${PROJECT}.hls"

MISC="readme.txt \
	${PROJECT}.rep \
	${PROJECT}_cnt.txt"

OPTIONAL="${PROJECT}.gwk"


if [ ! -d ${OUTDIR} ]; then
  mkdir ${OUTDIR}
fi

for file in `echo ${PDFS} ${GERBERS} ${ASMFILES} ${MISC}`; do
  if [ -e ${file} ]; then
    mv ${file} ${OUTDIR}/
  fi
  if [ ! -e ${OUTDIR}/${file} ]; then
    echo "${OUTDIR}/${file} missing!"
  fi
done


ZIPFILES=""
for file in `echo ${PDFS} ${GERBERS} ${ASMFILES} ${MISC} ${OPTIONAL}`; do
  ZIPFILES+=" ${OUTDIR}/${file}"
done

zip -r ${PROJECT}_${OUTDIR}.zip ${ZIPFILES}
