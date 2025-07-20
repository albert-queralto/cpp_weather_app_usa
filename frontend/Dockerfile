FROM node:20

WORKDIR /app

COPY frontend/ .

RUN npm install && \
    npm run build && \
    npm install -g serve

EXPOSE 3000
CMD ["serve", "-s", "build", "-l", "3000"]