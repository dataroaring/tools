while IFS=' ' read -r tablet version; do
  echo "$tablet $version"
  for be in `cat be_ips.txt`; do
      curl $be:8040/api/pad_rowset?tablet_id=$tablet&start_version=$version&end_version=$version
  done
done <padding.txt
