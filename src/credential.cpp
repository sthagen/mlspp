#include "mls/credential.h"
#include <tls/tls_syntax.h>

namespace mls {

///
/// BasicCredential
///

const CredentialType BasicCredential::type = CredentialType::basic;

///
/// X509Credential
///

X509Credential::X509Credential(const std::vector<bytes>& der_chain)
{
  if (der_chain.empty()) {
    throw std::invalid_argument("empty certificate chain");
  }

  for (const auto& der : der_chain) {
    chain.emplace_back(der);
  }
}

///
/// Credential
///

bytes
Credential::identity() const
{
  switch (_cred.index()) {
    case 0:
      return std::get<BasicCredential>(_cred).identity;
  }

  throw std::bad_variant_access();
}

SignaturePublicKey
Credential::public_key() const
{
  switch (_cred.index()) {
    case 0:
      return std::get<BasicCredential>(_cred).public_key;
  }

  throw std::bad_variant_access();
}

bool
Credential::valid_for(const SignaturePrivateKey& priv) const
{
  return priv.public_key == public_key();
}

Credential
Credential::basic(const bytes& identity, const SignaturePublicKey& public_key)
{
  Credential cred;
  cred._cred = BasicCredential{ identity, public_key };
  return cred;
}

Credential
Credential::x509(const std::vector<bytes>& der_chain)
{
  if (der_chain.empty()) {
    throw std::invalid_argument("empty cert chain");
  }

  auto certs = std::vector<hpke::Certificate>();
  certs.emplace_back(bytes{});

  Credential cred;
  // X509Credential x509Credential{der_chain};
  cred._cred = X509Credential{ der_chain };
  return cred;
}

} // namespace mls
