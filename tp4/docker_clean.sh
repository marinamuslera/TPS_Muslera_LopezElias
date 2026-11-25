docker rmi --force $(docker images | grep tp4_udesa | tr -s ' ' | cut -d ' ' -f 3)
