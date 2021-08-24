const { writeFile } = require('fs');
const { argv } = require('yargs');

// Load environment variables
require('dotenv').config();

// Read command line arguments
const env = argv.environment;
const isProd = env === 'prod';

// If required env variable are not loaded, quit with error
if (!process.env.PUBLISH_KEY || !process.env.SUBSCRIBE_KEY) {
  console.error('All the required environment variables were not provided!');
  process.exit(-1);
}

const targetPath = isProd
    ? `./src/environments/environment.prod.ts`
    : `./src/environments/environment.ts`;

// Build environment output file
const envFileContent = `
    export const environment = {
        production: ${isProd},
        PUBLISH_KEY: '${process.env.PUBLISH_KEY}',
        SUBSCRIBE_KEY: '${process.env.SUBSCRIBE_KEY}'
    };
`;

// Write environment data to file
writeFile(targetPath, envFileContent, (err: any) => {
    if (err) {
        console.log(err);
    }

    console.log(`Environment variables written to ${targetPath}`);
})