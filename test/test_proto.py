import smtplib
import email.utils
from email.mime.text import MIMEText
import pytest
import shutil
import os
import time

def test_ok():
# Create the message
    msg = MIMEText('This is the body of the message.')
    msg['To'] = email.utils.formataddr(('Recipient', 'recipient@example.com'))
    msg['From'] = email.utils.formataddr(('Author', 'author@example.com'))
    msg['Subject'] = 'Simple test message'

    server = smtplib.SMTP('127.0.0.1')
    server.set_debuglevel(True) # show communication with the server
    result = 0
    try:
        server.sendmail('author@localhost', ['recipient@example.com'], msg.as_string())
        result = 1
    finally:
        server.quit()
    assert result == 1

def test_verify():
# Create the message
    msg = MIMEText('This is the body of the message.')
    msg['To'] = email.utils.formataddr(('Recipient', 'recipient@example.com'))
    msg['From'] = email.utils.formataddr(('Author', 'author@example.com'))
    msg['Subject'] = 'Simple test message'

    server = smtplib.SMTP('127.0.0.1')
    server.set_debuglevel(True) # show communication with the server
    res = server.verify("vasya@rambler.ru")
    assert res[0] == 500
    res = server.ehlo("test")
    assert res[0] == 250
    res = server.verify("vasya@rambler.ru")
    assert res[0] == 550

def test_dns_fail():
# Create the message
    msg = MIMEText('This is the body of the message.')
    msg['To'] = email.utils.formataddr(('Recipient', 'recipient@example.com'))
    msg['From'] = email.utils.formataddr(('Author', 'author@example.com'))
    msg['Subject'] = 'Simple test message'

    server = smtplib.SMTP('127.0.0.1')
    server.ehlo("test")
    result = 0
    server.set_debuglevel(True) # show communication with the server
    try:
        server.sendmail('BHYK@yandex.ru', ['recipient@example.com'], msg.as_string())
    except smtplib.SMTPServerDisconnected:
        result = 1

    assert result == 1

def test_delivery():
    try:
        shutil.rmtree('/home/bhychik/local_mailbox/')
    except Exception:
        pass
    finally:
        pass
    try:
        shutil.rmtree('/home/bhychik/remote_mailbox/')
    except Exception:
        pass
    finally:
        pass

    msg = MIMEText('This is the body of the message.')
    msg['To'] = email.utils.formataddr(('Recipient', 'recipient@example.com'))
    msg['From'] = email.utils.formataddr(('Author', 'author@example.com'))
    msg['Subject'] = 'Simple test message'

    server = smtplib.SMTP('127.0.0.1')
    server.set_debuglevel(True) # show communication with the server
    result = 0
    try:
        server.sendmail('author@localhost', ['bhychik@yandex.ru', 'isaev@rambler.ru', 'isaev@rambler.ru', 'i.remen@corp.mail.ru', 'isaev@example.com', 'IsAEv@example.com'], msg.as_string())
        result = 1
    finally:
        server.quit()
    assert result == 1

    time.sleep(1)

    result = 0
    for dirname, dirnames, filenames in os.walk('/home/bhychik/local_mailbox/bhychik@yandex.ru/tmp'):
        if len(dirnames) == 0 and len(filenames) == 0:
            result = 1
    assert result == 1

    result = 0
    for dirname, dirnames, filenames in os.walk('/home/bhychik/local_mailbox/bhychik@yandex.ru/new'):
        if len(dirnames) == 0 and len(filenames) == 1:
            result = 1
    assert result == 1

    result = 0
    for dirname, dirnames, filenames in os.walk('/home/bhychik/local_mailbox/isaev@rambler.ru/tmp'):
        if len(dirnames) == 0 and len(filenames) == 0:
            result = 1
    assert result == 1

    result = 0
    isaev_mail_path = ""
    for dirname, dirnames, filenames in os.walk('/home/bhychik/local_mailbox/isaev@rambler.ru/new'):
        if len(dirnames) == 0 and len(filenames) == 1:
            result = 1
        isaev_mail_path = dirname + '/' + filenames[0]
    assert result == 1

    ln = 0
    for i in open(isaev_mail_path):
        if ln == 0:
            assert i == "Return-Path: author@localhost\r\n"
        elif ln == 1:
            assert i == "Delivered-To: isaev@rambler.ru\r\n"

        ln = ln + 1
        if ln == 2:
            break

    result = 0
    for dirname, dirnames, filenames in os.walk('/home/bhychik/remote_mailbox/tmp'):
        if len(dirnames) == 0 and len(filenames) == 0:
            result = 1
    assert result == 1

    result = 0
    for dirname, dirnames, filenames in os.walk('/home/bhychik/remote_mailbox/new'):
        if len(dirnames) == 0 and len(filenames) == 1:
            result = 1
        remote_path = dirname + '/' + filenames[0]
    assert result == 1

    ln = 0
    for i in open(remote_path):
        if ln == 0:
            assert i == "Return-Path: author@localhost\r\n"
        elif ln == 1:
            assert i == "Delivered-To: i.remen@corp.mail.ru, isaev@example.com\r\n"

        ln = ln + 1
        if ln == 2:
            break

    for dirname, dirnames, filenames in os.walk('/home/bhychik/local_mailbox/'):
        if dirname == '/home/bhychik/local_mailbox/':
            assert len(dirnames) == 2
            assert len(filenames) == 0
            break
