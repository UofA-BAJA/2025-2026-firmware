FROM mcr.microsoft.com/dotnet/sdk:9.0

WORKDIR /app

# copy everything
COPY . ./

# install dependencies with ?restore?
RUN dotnet restore

# expose port
EXPOSE 5269

ENV PORT=5269

# run the app 
CMD ["dotnet", "run", "--urls", "http://0.0.0.0:5269"]
