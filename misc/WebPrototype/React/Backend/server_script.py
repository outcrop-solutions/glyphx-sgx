import paramiko

def start_connection():
    u_name = 'ec2-user'
    pswd = ''
    port = 22
    r_ip = 'ec2-34-221-39-241.us-west-2.compute.amazonaws.com'
    sec_key = './webviewerinstance_rsa1.ppk'

    myconn = paramiko.SSHClient()
    myconn.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    my_rsa_key = paramiko.RSAKey.from_private_key_file(sec_key)

    session = myconn.connect(r_ip, username=u_name, password=pswd, port=port, pkey=my_rsa_key)

    remote_cmd = '''
    cd WebViewer/
    ./webViewerServer.sh -k
    ./webViewerServer.sh -s
    '''
    (stdin, stdout, stderr) = myconn.exec_command(remote_cmd)
    print(stdout.readlines())
    myconn.close()


if __name__ == '__main__':
    start_connection()