#!/usr/bin/env python

import os
import logging
import smtplib

from_email = os.environ['postbox_from_email']
to_emails = os.environ['postbox_to_emails'].split(';')

smtp_server = os.environ['postbox_smtp_server']
smtp_server_port = os.environ['postbox_smtp_server_port']
smtp_username = os.environ['postbox_smtp_username']
smtp_password = os.environ['postbox_smtp_password']

logging.basicConfig(level=logging.DEBUG)

logging.info('Starting up!')

logging.debug('Send from: '+from_email)
logging.debug('Send to: '+str(to_emails))
logging.debug('Send using: '+smtp_username+':'+smtp_password+'@'+smtp_server+':'+smtp_server_port)


def sendMail(subject):

    email_text = 'Subject: %s' % (subject)

    logging.debug('Mail to send: '+ email_text)
    
    try:
        logging.debug('Connecting to SMTP')

        server = smtplib.SMTP(smtp_server, smtp_server_port)
        server.ehlo()
        server.starttls()

        logging.debug('Login to SMTP')

        server.login(smtp_username, smtp_password)

        logging.debug('Sending mail')

        server.sendmail(from_email, to_emails, email_text)

        logging.debug('Mail sent!')

        server.close()

    except Error as e:  
        logging.error(e)

sendMail('Postbox was read!')
