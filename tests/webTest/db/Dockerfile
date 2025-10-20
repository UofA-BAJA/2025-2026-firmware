FROM postgres:18

# env vars
ENV POSTGRES_USER=myuser
ENV POSTGRES_PASSWORD=mysecretpassword
ENV POSTGRES_DB=mydatabase

# copy initialization SQL scripts
COPY ./schema/ /docker-entrypoint-initdb.d/

# expose default container port
EXPOSE 5432