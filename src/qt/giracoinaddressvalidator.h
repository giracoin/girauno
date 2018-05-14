// Copyright (c) 2011-2014 The Giracoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GIRACOIN_QT_GIRACOINADDRESSVALIDATOR_H
#define GIRACOIN_QT_GIRACOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class GiracoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit GiracoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Giracoin address widget validator, checks for a valid giracoin address.
 */
class GiracoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit GiracoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // GIRACOIN_QT_GIRACOINADDRESSVALIDATOR_H
