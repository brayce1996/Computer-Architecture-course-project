time ././golden/pipeline
mv error_dump.rpt error_dump.ans.rpt
mv snapshot.rpt snapshot.ans.rpt
time ./../simulator/pipeline
diff error_dump.ans.rpt error_dump.rpt >error.diff
diff snapshot.ans.rpt snapshot.rpt >Snap.diff
