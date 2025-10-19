# Use the latest LTS version of Node.js
FROM node:20.19
 
# Set the working directory inside the container
WORKDIR /app
 
# Copy package.json and package-lock.json
COPY package*.json ./
 
# Install dependencies
RUN npm install
 
# Copy the rest of your application files
COPY . .

ENV PORT=5173 

# Expose the port your app runs on
EXPOSE 5173
 
# Define the command to run your app (only 1 docker files per ?folder? allowed)
CMD ["npm", "run", "dev"]