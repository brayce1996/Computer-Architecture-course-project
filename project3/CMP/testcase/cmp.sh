time ./go_sim 256 256 32 32 16 4 4 16 4 4
mv report.rpt report.ans.rpt
mv snapshot.rpt snapshot.ans.rpt
time ./my_sim 256 256 32 32 16 4 4 16 4 4
diff report.ans.rpt report.rpt >Error.diff
diff snapshot.ans.rpt snapshot.rpt >Snap.diff
