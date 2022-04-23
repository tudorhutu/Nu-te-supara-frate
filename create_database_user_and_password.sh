
sudo -u postgres psql -c "CREATE DATABASE th;"
sudo -u postgres psql -c "CREATE USER postgres WITH ENCRYPTED PASSWORD 'myPassword';" 
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE th TO postgres;"

