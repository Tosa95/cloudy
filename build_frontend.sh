#!/bin/bash
curr_dir=$(pwd)
cd frontend/esp_experiments
npm run build
cd $curr_dir
rm data/report.html || true
for i in `find data | grep -E "\.css$|\.html$|\.js$"`; do gzip -9 -v "$i" ; done