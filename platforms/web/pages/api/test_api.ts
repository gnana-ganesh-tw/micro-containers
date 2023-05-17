// Next.js API route support: https://nextjs.org/docs/api-routes/introduction
import type { NextApiRequest, NextApiResponse } from 'next'
import { exec, execSync } from 'child_process'

type Data = {
  cmd_res: string
}

export default function handler(
  req: NextApiRequest,
  res: NextApiResponse<Data>
) {
  let cmd = JSON.parse(req.body).cmd;
  console.log(cmd);
  const output = exec(cmd, (error, stdout, stderr) => {
    if (error) {
      console.error(`exec error: ${error}`);
      return;
    }
    console.log(`stdout: ${stdout}`);
    console.error(`stderr: ${stderr}`);
    const filtered = stdout.toString();
    res.status(200).json({ cmd_res: filtered })
  });
}
